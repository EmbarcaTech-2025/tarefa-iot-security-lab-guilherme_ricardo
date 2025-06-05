/**
 * @file    mqtt_base.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Conecta ou Disconecta com o Broker MQTT
 * @version 0.1
 * @date    2025-06-04
 * 
 */
#ifndef MQTT_BASE_H
#define MQTT_BASE_H

#include "pico/stdlib.h" 

/**
 * @brief Disconecta do Broker MQTT
 * 
 * Coloca a máquina de estados no estado para iniciar a desconexão
 * 
 */
void mqtt_base_disconnect();

/**
 * @brief Baseado no estado da conexão WiFi, conecta ou disconecta do Broker
 * 
 * Máquina de estados para conexão e descconexão do Broker
 * 
 * @param wifi_connected, estado da conexão WiFi 
 * @return true,  indica que está conectado ao Broker
 * @return false, indica que está desconectado ao Broker 
 */
bool mqtt_base_update(bool wifi_connected);

#endif // MQTT_BASE_H