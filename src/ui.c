/**
 * @file    ui.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Gerencia a interface do usuário: LED RGB e Display OLED
 * @version 0.1
 * @date    2025-06-04
 * 
 */

#include "hardware/pwm.h"
#include "../include/ui.h"
#include "../include/config.h"
#include "../libs_ext/I2C.h"
#include "../libs_ext/Display.h"

typedef struct {
    bool      valid;
    bool      printed;
    int32_t   time_stamp;
    bool      time_stamp_valid;
    char      msg[32+1];
    bool      error;
} UiMsg;


static void ui_msg_copy(UiMsg * src, UiMsg * dest){
    dest->valid            = src->valid;
    dest->printed          = src->printed;
    dest->time_stamp       = src->time_stamp;
    dest->time_stamp_valid = src->time_stamp_valid;
    dest->error            = src->error;
    memcpy(dest->msg, src->msg, 32+1);
}

static          uint8_t  tick                  = 0;
static          uint64_t last_tick             = 0;
static volatile bool     last_wifi_connected   = false;
static volatile bool     last_broker_connected = false;
static volatile int      n_msg_published       = 0; 
static volatile int      n_msg_subscriber      = 0; 

static int32_t published_tc_max  = -1;
static int32_t subscriber_tc_max = -1;

#define N_MSGS 6

// Lista de mensagens enviadas e recebidas
static UiMsg ui_list_msg_sent[    N_MSGS];
static UiMsg ui_list_msg_received[N_MSGS];

/**
 * @brief Callback chamado a cada 100ms para atualizar o estado dos LEDs RGB
 * 
 */
static void ui_tick(){
    pwm_clear_irq(PWM_TICK); //  limpa o Irq senão no return desta rotina nova interrupção será gerada.

    if(time_us_64() < last_tick) return;

    last_tick = time_us_64() + UI_UPDATE_TICK_US;

    // blink blue
    if(tick == 0){ 
        pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_B, last_wifi_connected? PWM_LEVEL_B: 0);
    }else if(tick == 2){ // blink blue
        pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_B, last_broker_connected? PWM_LEVEL_B: 0);
    } else pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_B, 0);

    // blink red
    if(tick == 4){
        if(n_msg_subscriber > 0) {
            n_msg_subscriber--;
            pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_R, PWM_LEVEL_R);
        }
    }else pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_R, 0);

    // blink green
    if(tick == 7){
        if(n_msg_published > 0) {
            n_msg_published--;
            pwm_set_chan_level(PWM_SLICE_G, PWM_CHANNEL_G, PWM_LEVEL_G);
        }
    }else pwm_set_chan_level(PWM_SLICE_G, PWM_CHANNEL_G, 0);

    if((++tick) >= 10) tick = 0;
}

/**
 * @brief Inicializa os componenstes da interface
 * 
 */
void ui_init(){
    for(int i = 0; i< N_MSGS; i++){
        ui_list_msg_sent[i].valid = false;
    }
    for(int i = 0; i< N_MSGS; i++){
        ui_list_msg_received[i].valid = false;
    }

    i2c_bitdoglab_init();
    display_init();

    gpio_init(   GPIO_BUTTON_A);
    gpio_init(   GPIO_BUTTON_B);
    gpio_pull_up(GPIO_BUTTON_A);
    gpio_pull_up(GPIO_BUTTON_B);

   
    pwm_set_clkdiv_int_frac4(PWM_TICK,      250, 0);
    pwm_set_clkdiv_int_frac4(PWM_SLICE_R_B, 125, 0);
    pwm_set_clkdiv_int_frac4(PWM_SLICE_G,   125, 0);

    gpio_set_function(GPIO_LED_R, GPIO_FUNC_PWM);
    gpio_set_function(GPIO_LED_G, GPIO_FUNC_PWM);
    gpio_set_function(GPIO_LED_B, GPIO_FUNC_PWM);

    pwm_set_wrap(      PWM_TICK,      49999);
    pwm_set_wrap(      PWM_SLICE_R_B, PWM_MAX_LEVEL - 1);
    pwm_set_wrap(      PWM_SLICE_G,   PWM_MAX_LEVEL - 1);

    pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_R, 0);
    pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_B, 0);
    pwm_set_chan_level(PWM_SLICE_G,   PWM_CHANNEL_G, 0);

    pwm_set_enabled(PWM_SLICE_R_B, true);
    pwm_set_enabled(PWM_SLICE_G,   true);


    display_fill(     true, true);
    sleep_ms(LED_TEST_TIME_MS);
    display_fill(     false, true);
    sleep_ms(LED_TEST_TIME_MS);
    display_fill(     true, true);
    sleep_ms(LED_TEST_TIME_MS);
    display_fill(     false, true);
    display_msg(false, 4, 2, "IOT MQTT");
    display_msg(true,  4, 4, "SECURITY");

    pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_R, PWM_LEVEL_R);
    sleep_ms(LED_TEST_TIME_MS);
    pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_R, 0);
    pwm_set_chan_level(PWM_SLICE_G,   PWM_CHANNEL_G, PWM_LEVEL_G);
    sleep_ms(LED_TEST_TIME_MS);
    pwm_set_chan_level(PWM_SLICE_G,   PWM_CHANNEL_G, 0);
    pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_B, PWM_LEVEL_B);
    sleep_ms(LED_TEST_TIME_MS);
    pwm_set_chan_level(PWM_SLICE_R_B, PWM_CHANNEL_B, 0);

    // ativa tick de 100ms
    pwm_clear_irq(            PWM_TICK);
    pwm_set_irq_enabled(      PWM_TICK, true);

    irq_set_exclusive_handler(PWM_IRQ_WRAP, ui_tick);
    irq_clear(                PWM_IRQ_WRAP);
    irq_set_enabled(          PWM_IRQ_WRAP, true);
    pwm_set_enabled(          PWM_TICK, true);
}


uint64_t t_last_oled_update = 0;
/**
 * @brief Atualiza o estado da interface baseado no extado da conexão do WiFI e Broker, bem como atualiza o display OLED quando for o caso
 * 
 * @param wifi_connected 
 * @param broker_connected 
 */
void ui_update(bool wifi_connected, bool broker_connected){
    last_wifi_connected   = wifi_connected;
    last_broker_connected = broker_connected;

    // Reporta no terminal as ultimas mensagens enviadas, que ainda não foram para o termina
    for(int i = N_MSGS-1; i >= 0;i--){
        if(ui_list_msg_sent[i].valid){
            if(!(ui_list_msg_sent[i].printed)){
                ui_list_msg_sent[i].printed = true;
                printf("MSG Sent: %s\n", ui_list_msg_sent[i].msg);
            }
        }    
    }

    // Reporta no terminal as ultimas mensagens recebidas, que ainda não foram para o termina
    for(int i = N_MSGS-1; i >= 0;i--){
        if(ui_list_msg_received[i].valid){
            if(!(ui_list_msg_received[i].printed)){
                ui_list_msg_received[i].printed = true;
                printf("MSG Received: %s\n", ui_list_msg_received[i].msg);
            }
        }    
    }

    // Periodicamente atualiza o display OLED
    if(time_us_64() > (t_last_oled_update + 100000)){
        t_last_oled_update = time_us_64();

        if(last_wifi_connected && last_broker_connected){
            display_fill(false, false);
            display_set_ret(true, false, 63,  0,  63, 63);
            display_set_ret(true, false, 64,  0,  64, 63);
            display_set_ret(true, false,  0, 11, 127, 11);
            display_set_ret(true, false,  0, 12, 127, 12);
            display_set_ret(true, false, 19, 12,  19, 63);
            display_set_ret(true, false, 91, 12,  91, 63);
            display_msg(false,  2, 0, "PUB");
            display_msg(false, 11, 0, "SUB");
            /*
            display_msg(false,  0, 2, "12");
            display_msg(false,  3, 2, "3456");
            display_msg(false,  9, 2, "12");
            display_msg(false, 12, 2, "3456");*/

            char c_tmp[4+1] = {0};
            for(int i=0;i<N_MSGS;i++){
                if(ui_list_msg_sent[i].valid){
                    sprintf(c_tmp, "%02d", ui_list_msg_sent[i].time_stamp % 100);
                    display_msg(false,  0, 2+i, c_tmp);
                    if(!ui_list_msg_sent[i].time_stamp_valid){
                        display_invert_char(false, 0, 2+i);
                        display_invert_char(false, 1, 2+i);
                    }

                    strncpy(c_tmp, ui_list_msg_sent[i].msg, 4);
                    display_msg(false,  3, 2+i, c_tmp);
                }
                if(ui_list_msg_received[i].valid){
                    if(ui_list_msg_received[i].error){
                        strncpy(c_tmp, ui_list_msg_received[i].msg, 4);
                        display_msg(false, 12, 2+i, "ERRO");
                        display_invert_char(false, 12, 2+i);
                        display_invert_char(false, 13, 2+i);
                        display_invert_char(false, 14, 2+i);
                        display_invert_char(false, 15, 2+i);
                    }else{
                        sprintf(c_tmp, "%02d", ui_list_msg_received[i].time_stamp % 100);
                        display_msg(false,  9, 2+i, c_tmp);
                        if(!ui_list_msg_received[i].time_stamp_valid){
                            display_invert_char(false,  9, 2+i);
                            display_invert_char(false, 10, 2+i);
                        }
                        strncpy(c_tmp, ui_list_msg_received[i].msg, 4);
                        display_msg(false, 12, 2+i, c_tmp);
                    }    
                }
            }

            display_update();
        }else{
            display_fill(false, false);
            display_msg(false, 3, 2, last_wifi_connected?   "WIFI:   ON":"WIFI:   OFF");
            display_msg(true,  3, 4, last_broker_connected? "BROKER: ON":"BROKER: OFF");
        }
    }
}

/**
 * @brief Informa que uma mensagem foi enviada, published, para piscar o led de enviou e coloca-la no display
 * 
 * @param tc 
 * @param value 
 */
void ui_msg_sent(int32_t tc, int32_t value){
    // Atualiza a pilha de mensagens enviadas
    for(int i = N_MSGS -2; i >= 0;i--){
        ui_msg_copy(&ui_list_msg_sent[i], &ui_list_msg_sent[i+1]);
    }
    ui_list_msg_sent[0].valid            = true;
    ui_list_msg_sent[0].printed          = false;
    ui_list_msg_sent[0].time_stamp       = tc;
    ui_list_msg_sent[0].time_stamp_valid = false;

    if(published_tc_max < tc){
        published_tc_max = tc;
        ui_list_msg_sent[0].time_stamp_valid = true;
    }
    sprintf(ui_list_msg_sent[0].msg, "%d", value);

    // incrementa o número de imagens enviadas
    n_msg_published++;
}

/**
 * @brief Informa que uma mensagem foi recebida, subscribed, para piscar o led de recebimento e coloca-la no display
 * 
 * @param tc 
 * @param value 
 * @param valid 
 */
void ui_msg_received(int32_t tc, int32_t value, bool valid){
    // Atualiza a pilha de mensagens recebidas
    for(int i = N_MSGS -2; i >= 0;i--){
        ui_msg_copy(&ui_list_msg_received[i], &ui_list_msg_received[i+1]);
    }
    ui_list_msg_received[0].valid            = true;
    ui_list_msg_received[0].printed          = false;
    ui_list_msg_received[0].time_stamp       = valid?tc:0;
    ui_list_msg_received[0].time_stamp_valid = false;
    sprintf(ui_list_msg_received[0].msg, "%d", valid?value:0);

    if(valid){
        if(subscriber_tc_max < tc){
            subscriber_tc_max = tc;
            ui_list_msg_received[0].time_stamp_valid = true;
        }
    }
    ui_list_msg_received[0].error = !valid;

    // incrementa o número de mensagens recebidas
    n_msg_subscriber++;
}