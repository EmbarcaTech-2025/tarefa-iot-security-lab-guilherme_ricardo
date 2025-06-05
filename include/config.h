/**
 * @file    config.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Define os principais parâmetros que serão utilizados pelo software
 * @version 0.1
 * @date    2025-06-04
 * 
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "pico/stdlib.h"
#include "lwip/apps/mqtt.h" 
#include "pico/cyw43_arch.h"  

// #define BITDOGLAB_WITH_DEBUG_PROBE       true

#define SW_PRINT_DEBUG                   false
#define WIFI_DEINIT_DELAY_MS               500
#define WIFI_AFTER_CONNECT_FAIL_DELAY_MS  2000
#define WIFI_CONNECTION_TIMEOUT_MSW      30000
#define BROKER_PORT                       1883
#define PUBLICHER_UPDATE_BUTTONS_US     250000

#define GPIO_BUTTON_A 5
#define GPIO_BUTTON_B 6

#define GPIO_LED_R  13      // PWB 6 B
#define GPIO_LED_G  11      // PWB 5 B
#define GPIO_LED_B  12      // PWB 6 A

#define PWM_TICK        4
#define PWM_DIV         125
#define PWM_SLICE_R_B   6
#define PWM_SLICE_G     5
#define PWM_CHANNEL_R   1
#define PWM_CHANNEL_G   1
#define PWM_CHANNEL_B   0
#define PWM_MAX_LEVEL   100

#define PWM_LEVEL_R     10
#define PWM_LEVEL_G     10
#define PWM_LEVEL_B     10

#define LED_TEST_TIME_MS  500
#define UI_UPDATE_TICK_US 100000

#define BITDOGLAB_I2C_BUS               i2c1
#define BITDOGLAB_I2C_BUS_BAUDRATE      400000
#define BITDOGLAB_I2C_BUS_GPIO_SCL      15
#define BITDOGLAB_I2C_BUS_GPIO_SDA      14


typedef enum {
    CRYPTO_MODE_NO_CRYPTO = 0,
    CRYPTO_MODE_CRYPTO_XOR,
    //CRYPTO_MODE_CRYPTO_AES,
    CRYPTO_MODE_N_MODES
} CryptoMode;


extern char       wifi_ssid[];
extern char       wifi_psw[];

extern ip4_addr_t device_brocker_ip;
extern char       device_id[];
extern char       device_user[];
extern char       device_psw[];

extern bool       publisher_enable;
extern char       publisher_topic[];
extern int        publisher_qos;
extern int        publisher_repeat_after_ms;
extern CryptoMode publisher_crypto_mode;
extern bool       publisher_msg_with_timetick;

extern bool       subscriber_enable;
extern char       subscriber_topic[];
extern int        subscriber_qos;
extern CryptoMode subscriber_crypto_mode;
extern bool       subscriber_msg_with_timetick;

/**
 * @brief Menu tty para configurar os parâmetros que serão utilizados
 * 
 */
void config_set();

#endif // CONFIG_H
