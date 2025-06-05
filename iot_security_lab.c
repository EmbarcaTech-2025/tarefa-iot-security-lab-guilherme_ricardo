#include <string.h>                 // Para funções de string como strlen()
#include "pico/stdlib.h"            // Biblioteca padrão do Pico (GPIO, tempo, etc.)
#include "include/config.h"
#include "include/ui.h"
#include "include/wifi.h"
#include "include/mqtt_base.h" 


int main() {
    // Inicializa todas as interfaces de I/O padrão (USB serial, etc.)
    stdio_init_all();

#ifdef BITDOGLAB_WITH_DEBUG_PROBE
    gpio_set_function(0, GPIO_FUNC_SIO);
    gpio_set_function(1, GPIO_FUNC_SIO);
    gpio_set_function(17, GPIO_FUNC_UART);
    gpio_set_function(16, GPIO_FUNC_UART);
#else    
    sleep_ms(7000);
#endif

    printf("Init program\n");
    ui_init();

    // Main loop1, mantem o config disponivel durante a execucao do programa
    while(1){
        config_set();
        printf("Config done, starting program, PRESS Any key to return to config\n");
        bool executing = true;

        // Main loop2 mantem o programa executando
        bool wifi_connected   = false;
        bool broker_connected = false;
        while (executing){
            
            if(wifi_connected){
                //cyw43_arch_poll();
                if(cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP){
                    // minhas rotinas com wifi conectado
                    //broker_connected = mqtt_base_update();
                }else{
                    wifi_connected = false;
                    broker_connected = false;
                    mqtt_base_disconnect();
                    sleep_ms(WIFI_AFTER_CONNECT_FAIL_DELAY_MS);
                }
            }else{
                printf("WiFi desconnected\n");
                wifi_connected = wifi_connect_or_reconnected();
                if (!wifi_connected) {
                    sleep_ms(WIFI_AFTER_CONNECT_FAIL_DELAY_MS);  // Pequena espera antes de tentar de novo
                }
            }
            broker_connected = mqtt_base_update(wifi_connected);

            // se precionar alguma tecla no terminal volta para o menu de configuração
            if(getchar_timeout_us(0) != PICO_ERROR_TIMEOUT){
                executing = false;
                mqtt_base_disconnect();
                broker_connected = false;
                wifi_disconnect();
                wifi_connected   = false;
            }

            ui_update(wifi_connected, broker_connected);
        }
    }
    return 0;
}

/* 
 * Comandos de terminal para testar o MQTT:
 * 
 * Inicia o broker MQTT com logs detalhados:
 * mosquitto -c mosquitto.conf -v
 * 
 * Assina o tópico de temperatura (recebe mensagens):
 * mosquitto_sub -h localhost -p 1883 -t "escola/sala1/temperatura" -u "aluno" -P "senha123"
 * 
 * Publica mensagem de teste no tópico de temperatura:
 * mosquitto_pub -h localhost -p 1883 -t "escola/sala1/temperatura" -u "aluno" -P "senha123" -m "26.6"
 */
 
 
 
 
