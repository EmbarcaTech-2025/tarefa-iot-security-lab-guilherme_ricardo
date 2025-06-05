/**
 * @file    ui.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Gerencia a interface do usuário: LED RGB e Display OLED
 * @version 0.1
 * @date    2025-06-04
 * 
 */
#ifndef UI_H
#define UI_H

#include "pico/stdlib.h"

/**
 * @brief Inicializa os componenstes da interface
 * 
 */
void ui_init();

/**
 * @brief Atualiza o estado da interface baseado no extado da conexão do WiFI e Broker, bem como atualiza o display OLED quando for o caso
 * 
 * @param wifi_connected 
 * @param broker_connected 
 */
void ui_update(bool wifi_connected, bool broker_connected);

/**
 * @brief Informa que uma mensagem foi enviada, published, para piscar o led de enviou e coloca-la no display
 * 
 * @param tc 
 * @param value 
 */
void ui_msg_sent(    int32_t tc, int32_t value);

/**
 * @brief Informa que uma mensagem foi recebida, subscribed, para piscar o led de recebimento e coloca-la no display
 * 
 * @param tc 
 * @param value 
 * @param valid 
 */
void ui_msg_received(int32_t tc, int32_t value, bool valid);


#endif // UI_H