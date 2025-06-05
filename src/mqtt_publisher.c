/**
 * @file    mqtt_publisher.c
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Gerencia o envio de mensagens pelo publisher
 * 
 * As mensagens podem ser  periódicas  e repetidas ou mensagens atualizadas comandadas pelos botões A ou B
 * 
 * @version 0.1
 * @date    2025-06-04
 * 
 */

#include <string.h>
#include "pico/stdlib.h"
#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP
#include "pico/cyw43_arch.h"  
#include "../include/mqtt_publisher.h"
#include "../include/cipher.h"
#include "../include/config.h"
#include "../include/mqtt_base.h"
#include "../include/ui.h"


static volatile u_int64_t t_last_publish;
static volatile bool      publisher_busy      = false;
static          char      last_sent_msg[32+1] = {0};
static          int32_t   last_sent_value     = 261;
static          int32_t   last_sent_tc        = 0;


 /**
  * @brief Callback de confirmação de publicação
  * 
  * @param arg,    argumento opcional
  * @param result, código de resultado da operação
  */
static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result == ERR_OK) {
        if(SW_PRINT_DEBUG) printf("Publish: Publicação MQTT enviada com sucesso!\n");
        ui_msg_sent(last_sent_tc, last_sent_value);
    } else {
        printf("Publisher: Erro ao publicar via MQTT: %d\n", result);
        //mqtt_publisher_disconnect();
        mqtt_base_disconnect();
        
    }
    publisher_busy = false;
}

/**
 * @brief Função para enviar/publicar efetivamente a mensagem
 * 
 * @param client 
 * @param topic,  nome do tópico (ex: "sensor/temperatura") 
 * @param data,   payload da mensagem (bytes)
 * @param data_le, tamanho do payload
 */
void mqtt_comm_publish(mqtt_client_t * client, const char *topic, uint8_t *data, u_int32_t data_len) {
    // Envia a mensagem MQTT
    publisher_busy = true;
    err_t status = mqtt_publish(
        client,              // Instância do cliente
        topic,               // Tópico de publicação
        data,                // Dados a serem enviados
        data_len,            // Tamanho dos dados
        0,                   // QoS 0 (nenhuma confirmação)
        0,                   // Não reter mensagem
        mqtt_pub_request_cb, // Callback de confirmação
        NULL                 // Argumento para o callback
    );

    if (status != ERR_OK) {
        printf("PUB: mqtt_publish falhou ao ser enviada: %d\n", status);
        mqtt_base_disconnect();
    } else{
        if(SW_PRINT_DEBUG) printf("PUB: mqtt_publish está tentando enviar: %s\n", data);
        t_last_publish = time_us_64();
    }
}


/**
 * @brief Inicializa o estado das variáveis do publisher
 * 
 */
void publisher_init(){
    publisher_busy = false;
}

static uint8_t data_sent[20];
/**
 * @brief Gera a mensagem que deve ser enviada
 * 
 * @param client 
 * @param tc 
 * @param value 
 */
static void publish_value(mqtt_client_t * client, int32_t tc, int32_t value){
    if(publisher_msg_with_timetick){
        cipher_msg_generate(data_sent, tc,  value);
    }else{
        sprintf(data_sent, "%4d", last_sent_value);
    }

    u_int32_t len = strlen(data_sent);

    switch(publisher_crypto_mode){
        case CRYPTO_MODE_NO_CRYPTO:
            break;
        case CRYPTO_MODE_CRYPTO_XOR:
            cipher_xor(data_sent, data_sent, len, 42);
            break;
        //case CRYPTO_MODE_CRYPTO_AES:
    }
    
    mqtt_comm_publish(client, publisher_topic, data_sent, len);
}

/**
 * @brief Gerencia o envio de mensagens pelo publisher
 * 
 * As mensagens podem ser  periódicas  e repetidas ou mensagens atualizadas comandadas pelos botões A ou B
 * 
 * @param client 
 */
void publisher_connected_update(mqtt_client_t *         client){
    if(publisher_busy) return;

    // checa se é para repetir a ultima msg
    if(time_us_64() > (t_last_publish + (publisher_repeat_after_ms * 1000))){
        publish_value(client, last_sent_tc, last_sent_value);
    // checa se é para atualizar botões
    }else if(time_us_64() > (t_last_publish + PUBLICHER_UPDATE_BUTTONS_US)){
        if(!gpio_get(GPIO_BUTTON_A)){
            publish_value(client, ++last_sent_tc, --last_sent_value);
        }else if(!gpio_get(GPIO_BUTTON_B)){
            publish_value(client, ++last_sent_tc, ++last_sent_value);
        }    
    }    
}