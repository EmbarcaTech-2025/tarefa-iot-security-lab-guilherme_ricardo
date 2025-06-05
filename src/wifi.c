/**
 * @file    wifi.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Conecta e Disconecta o módulo de WiFi do modem
 * @version 0.1
 * @date 2025-06-04
 * 
 */
#include "../include/config.h"
#include "../include/wifi.h"

static bool wifi_hw_was_inited = false;

/**
 * @brief Desinicializa o módulo de WiFi caso tenha sido inicializado previamente
 * 
 */
void wifi_disconnect() {
    // Desativa o Driver se previamente inicializado
    if(wifi_hw_was_inited){
        if(SW_PRINT_DEBUG) printf("DeInit WiFi Hardware\n");
        cyw43_arch_deinit();
        sleep_ms(WIFI_DEINIT_DELAY_MS);
        wifi_hw_was_inited = false;
    }
}

/**
 * @brief Tenta fazer a Conexão com o roteador WiFi
 * 
 * @return true, conseguiu conectar 
 * @return false, NÃO conseguiu conectar 
 */
bool wifi_connect_or_reconnected() {
    wifi_disconnect();

    if(SW_PRINT_DEBUG) printf("Init WiFi Hardware\n");
    wifi_hw_was_inited = true;
    if (cyw43_arch_init()) {
        if(SW_PRINT_DEBUG) printf("error initializing WiFi hardware\n");
        return false;
    }
    if(SW_PRINT_DEBUG) printf("Initialized WiFi Hardware\n");

    // Habilita o modo estação (STA) para se conectar a um ponto de acesso.
    cyw43_arch_enable_sta_mode();
    if(SW_PRINT_DEBUG) printf("WiFi STA mode enabled\n");

    // tenta fazer a conexão
    if (cyw43_arch_wifi_connect_timeout_ms(wifi_ssid, wifi_psw, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECTION_TIMEOUT_MSW)) {
        if(SW_PRINT_DEBUG) printf("WiFi connection FAIL\n");
        return false;
    }

    // Caso o IP obtido for 0.0.0.0 houve um erro
    if(cyw43_state.netif[0].ip_addr.addr == 0){
        if(SW_PRINT_DEBUG) printf("WiFi Invalid IP = 0.0.0.0\n");
        return false;
    }

    uint8_t *ip   = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
    printf("WiFi connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    
    return true;
}