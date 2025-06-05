/**
 * @file    wifi.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Conecta e Disconecta o módulo de WiFi do modem
 * @version 0.1
 * @date 2025-06-04
 * 
 */
#ifndef WIFI_H
#define WIFI_H

#include "pico/stdlib.h" 

/**
 * @brief Desinicializa o módulo de WiFi caso tenha sido inicializado previamente
 * 
 */
void wifi_disconnect();

/**
 * @brief Tenta fazer a Conexão com o roteador WiFi
 * 
 * @return true, conseguiu conectar 
 * @return false, NÃO conseguiu conectar 
 */
bool wifi_connect_or_reconnected();

#endif // WIFI_H

