/**
 * @file    mqtt_subscriber.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief 
 * @version 0.1
 * @date    2025-06-04
 */
#include <string.h>
#include "pico/stdlib.h"
#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP
#include "pico/cyw43_arch.h"  
#include "../include/mqtt_subscriber.h"
#include "../include/cipher.h"
#include "../include/config.h"
#include "../include/mqtt_base.h"
#include "../include/ui.h"


static volatile bool subscriber_is_connected = false;


/**
 * @brief Informa que o Subscriver recebeu uma mensagem de um determinado tópico
 * 
 * Não informa a mensagem
 * 
 * @param arg 
 * @param topic 
 * @param tot_len 
 */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    if(SW_PRINT_DEBUG) printf("Subscriber: Mensagem recebida no tópico: %s, tamanho: %lu\n", topic, tot_len);
}

static int32_t auto_count_msgs_in = 0;

/**
 * @brief Informa o dado recebido, como subscriber
 * 
 * @param arg 
 * @param data 
 * @param len 
 * @param flags 
 */
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    if(SW_PRINT_DEBUG) printf("Subscriber: Payload recebido: %.*s\n", len, data);

    char s_tmp[32+1] = {0};
    switch(subscriber_crypto_mode){
        case CRYPTO_MODE_NO_CRYPTO:
            if(len > 32) len = 32;
            memcpy(s_tmp, data, len);
            break;
        case CRYPTO_MODE_CRYPTO_XOR:
            if(len > 32) len = 32;
            memcpy(s_tmp, data, len);
            cipher_xor(s_tmp, s_tmp, len, 42);
            s_tmp[len] = 0;
            break;
        //case CRYPTO_MODE_CRYPTO_AES:
    }

    bool valid_data;
    int32_t tc, value;
    if(subscriber_msg_with_timetick){
        valid_data = cipher_msg_get(s_tmp, &tc,  &value);
        ui_msg_received(tc, value, valid_data);
    }else{
        valid_data = sscanf(s_tmp, "%d", &value) == 1;
        auto_count_msgs_in++;
        ui_msg_received(auto_count_msgs_in, value, valid_data);
    }
}

/**
 * @brief Calback que informa se foi possivel ou não o inscrição do Subscriber
 * 
 * @param arg 
 * @param err 
 */
static void mqtt_sub_request_cb(void *arg, err_t err) {
    if (err == ERR_OK) {
        printf("Subscriber: Inscrição (subscribe) realizada com sucesso.\n");
        subscriber_is_connected = true;
    } else {
        printf("Subscriber: Falha ao realizar inscrição. Código de erro: %d\n", err);
        mqtt_base_disconnect();
    }
}

/**
 * @brief Faz uma requisição para conectar o subscriber ao Broker
 * 
 * Obs.: mqtt_subscriber_is_connected vai informar quando efetivamente foi conectado ou n ainda não
 * 
 * @param client 
 */
void mqtt_subscriber_connect(mqtt_client_t * client){
    subscriber_is_connected = false;
    // após registrar no broquer
    if(SW_PRINT_DEBUG) printf("Subscriber - Set CallBack\n");
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);
    
    if(SW_PRINT_DEBUG) printf("Subscriber - Set\n");
    err_t err = mqtt_subscribe(client, subscriber_topic, 1, mqtt_sub_request_cb, NULL);
    if (err != ERR_OK) {
        printf("Subscriber: Erro ao fazer subscribe: %d\n", err);
        mqtt_base_disconnect();
    }
    printf("Subscriber - registrando\n");
}

/**
 * @brief Verifica se o Subscriber foi ou não conectado ao Broker
 * 
 * @return true,  Subscriver     está conectado ao Broker
 * @return false, Subscriver NÃO está conectado ao Broker 
 */
bool mqtt_subscriber_is_connected(){
    return subscriber_is_connected;
}

