/**
 * @file    mqtt_publisher.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Gerencia o envio de mensagens pelo publisher
 * 
 * As mensagens podem ser  periódicas  e repetidas ou mensagens atualizadas comandadas pelos botões A ou B
 * 
 * @version 0.1
 * @date    2025-06-04
 * 
 */
#ifndef MQTT_PUBLISHER_H
#define MQTT_PUBLISHER_H

#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP

/**
 * @brief Inicializa o estado das variáveis do publisher
 * 
 */
void publisher_init();

/**
 * @brief Gerencia o envio de mensagens pelo publisher
 * 
 * As mensagens podem ser  periódicas  e repetidas ou mensagens atualizadas comandadas pelos botões A ou B
 * 
 * @param client 
 */
void publisher_connected_update(mqtt_client_t * client);


#endif //  MQTT_PUBLISHER_H