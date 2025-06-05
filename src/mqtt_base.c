/**
 * @file    mqtt_base.c
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Conecta ou Disconecta com o Broker MQTT
 * @version 0.1
 * @date    2025-06-04
 * 
 */
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "../include/mqtt_base.h"
#include "../include/config.h"
#include "../include/mqtt_publisher.h"
#include "../include/mqtt_subscriber.h"


typedef enum {
    MBCS_NO_CONNECTION,             // Não está conectado ao broker
    MBCS_IN_CONNECTION,             // está tentando conectar no broker, há um timeout
    MBCS_CONNECTED_WAIT_SUBSCRIBE,  // está tentando conectar no broker como subscriber
    MBCS_CONNECTED,                 // está conectado ao broker
    MBCS_DISCONNECT_START,          // iniciando a desconexão do broker
    MBCS_DISCONNECT_IN,             // tempo necessário para desconectar efetivamento do broker
} MqttBaseConnectionState;


static MqttBaseConnectionState mbcs           = MBCS_NO_CONNECTION;
static mqtt_client_t *         client         = NULL;
static uint64_t                t_timeout_in_connection_us;


/**
 * @brief Informa se foi possivel conectar ao Broker ou não
 * 
 * Atualiza o estado da máquina de estados
 * 
 * @param client 
 * @param arg 
 * @param status 
 */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker MQTT com sucesso!\n");
        if(subscriber_enable){
            mqtt_subscriber_connect(client);
            t_timeout_in_connection_us = time_us_64();
            mbcs = MBCS_CONNECTED_WAIT_SUBSCRIBE;
        }else{
            mbcs = MBCS_CONNECTED;
        }
    } else {
        printf("Falha ao conectar ao broker, código: %d\n", status);
        mqtt_base_disconnect();
    }
}

/**
 * @brief Disconecta do Broker MQTT
 * 
 * Coloca a máquina de estados no estado para iniciar a desconexão
 * 
 */
void mqtt_base_disconnect(){
    // como essa rotina é chamada de dentro de uma interrupção, executar o disconnect original gerava PANIC, por isso foi criada uma maquina de estados para disconect
    
    mbcs = MBCS_DISCONNECT_START;
}

/**
 * @brief Baseado no estado da conexão WiFi, conecta ou disconecta do Broker
 * 
 * Máquina de estados para conexão e descconexão do Broker
 * 
 * @param wifi_connected, estado da conexão WiFi 
 * @return true,  indica que está conectado ao Broker
 * @return false, indica que está desconectado ao Broker 
 */
bool mqtt_base_update(bool wifi_connected){
        switch (mbcs){
        case MBCS_DISCONNECT_START:
            if(client != NULL){
                mqtt_disconnect(client);
                mqtt_client_free(client);
                client = NULL;
                if(SW_PRINT_DEBUG) printf("MBCS_DISCONNECT_START client != NULL\n");
            }
            if(SW_PRINT_DEBUG) printf("MBCS_DISCONNECT_START\n");
            mbcs = MBCS_DISCONNECT_IN;
            t_timeout_in_connection_us = time_us_64() + (WIFI_AFTER_CONNECT_FAIL_DELAY_MS * 1000);
            return false;

        case MBCS_DISCONNECT_IN:
            if(time_us_64() > t_timeout_in_connection_us){

                mbcs = MBCS_NO_CONNECTION;
            }
            return false;

        case MBCS_NO_CONNECTION:
            if(!wifi_connected)return false;
            client = mqtt_client_new();
            if (client == NULL) {
                printf("Falha ao criar o cliente MQTT\n");
                return false;
            }
            struct mqtt_connect_client_info_t ci = {
                .client_id   = device_id,       // ID do cliente
                .client_user = device_user,     // Usuário (opcional)
                .client_pass = device_psw       // Senha (opcional)
            };
            // Inicia a conexão com o broker
            // Parâmetros:
            //   - client: instância do cliente
            //   - &broker_addr: endereço do broker
            //   - 1883: porta padrão MQTT
            //   - mqtt_connection_cb: callback de status
            //   - NULL: argumento opcional para o callback
            //   - &ci: informações de conexão
            err_t err = mqtt_client_connect(client, &device_brocker_ip, BROKER_PORT, mqtt_connection_cb, NULL, &ci);
            if((err == ERR_OK) || (err == ERR_ISCONN)){
                printf("Tentando Conectar ao Broker\n");
                mbcs = MBCS_IN_CONNECTION;
                t_timeout_in_connection_us = time_us_64();
            }else{
                printf("Falha ao criar o cliente MQTT err = %d\n", err);
            }
            sleep_ms(100);
            publisher_init();
            return false;

        case MBCS_IN_CONNECTION:
            if(time_us_64() > (t_timeout_in_connection_us + 10000000)){
                printf("Timeout MQTT connection\n");
                mqtt_base_disconnect();
            }
            return false;

        case MBCS_CONNECTED_WAIT_SUBSCRIBE:
            if(time_us_64() > (t_timeout_in_connection_us + 10000000)){
                printf("Timeout MQTT connection wait subscriber\n");
                mqtt_base_disconnect();
            }else{
                if(mqtt_subscriber_is_connected()){
                    mbcs = MBCS_CONNECTED;
                }
            }
            return false;

        case MBCS_CONNECTED:
            // verificar conexão com broker?
            if(publisher_enable) publisher_connected_update(client);
            return true;        
    }
}