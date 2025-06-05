/**
 * @file    mqtt_subscriber.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief 
 * @version 0.1
 * @date    2025-06-04
 */
#ifndef MQTT_SUBSCRIBER_H
#define MQTT_SUBSCRIBER_H

#include "pico/stdlib.h"
#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP

/**
 * @brief Faz uma requisição para conectar o subscriber ao Broker
 * 
 * Obs.: mqtt_subscriber_is_connected vai informar quando efetivamente foi conectado ou n ainda não
 * 
 * @param client 
 */
void mqtt_subscriber_connect(mqtt_client_t * client);


/**
 * @brief Verifica se o Subscriber foi ou não conectado ao Broker
 * 
 * @return true,  Subscriver     está conectado ao Broker
 * @return false, Subscriver NÃO está conectado ao Broker 
 */
bool mqtt_subscriber_is_connected();



#endif // MQTT_SUBSCRIBER_H