/**
 * @file    config.h
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief   Define os principais parâmetros que serão utilizados pelo software
 * @version 0.1
 * @date    2025-06-04
 * 
 */
#include <string.h> 
#include "pico/stdlib.h" 
#include "pico/cyw43_arch.h"        // Driver WiFi para Pico W
#include "../include/config.h"

#define STRING_SIZE 32

char wifi_ssid[ STRING_SIZE+1] = "rede_wifi";
char wifi_psw[  STRING_SIZE+1] = "senha_rede_wifi";


ip4_addr_t device_brocker_ip                = {(u32_t)0x0E00A8C0};


char        device_id[   STRING_SIZE+1]    = {"pub_id_01"};
char        device_user[ STRING_SIZE+1]    = {"user01"};
char        device_psw[  STRING_SIZE+1]    = {"psw01"};


bool        publisher_enable                = true;
char        publisher_topic[STRING_SIZE+1]  = {"escola/sala1/temperatura"};
int         publisher_qos                   = 0;
int         publisher_repeat_after_ms       = 5000;
CryptoMode  publisher_crypto_mode           = CRYPTO_MODE_NO_CRYPTO;
bool        publisher_msg_with_timetick     = false;

bool        subscriber_enable;
char        subscriber_topic[STRING_SIZE+1] = {"escola/sala1/temperatura"};
int         subscriber_qos                  = 0;
CryptoMode  subscriber_crypto_mode          = CRYPTO_MODE_NO_CRYPTO;
bool        subscriber_msg_with_timetick    = false;


static void read_string(char * msg){
    bool fim = false;
    int pos = 0;
    while(!fim){
        char c = getchar();
        if((c =='\n')||(c =='\r')) fim = true;
        else{
            msg[pos] = c;
            printf("%c", c);
            pos++;
            if(pos == STRING_SIZE) fim = true;
        }
    }
    msg[pos] = 0;
}

/**
 * @brief Menu tty para configurar os parâmetros que serão utilizados
 * 
 */
void config_set(){
    bool fim = false;
    while(!fim){
        printf("\033[2J");
        printf("\033[H");
        printf("0) WiFI SSID             = %s\n", wifi_ssid);
        printf("1) WiFI PWS              = %s\n", wifi_psw);
        printf("\n");
        printf("2) Broker IP             = %s\n", ip4addr_ntoa(&device_brocker_ip));
        printf("3) Device ID             = %s\n", device_id);
        printf("4) Device User           = %s\n", device_user);
        printf("5) Device PSW            = %s\n", device_psw);
        printf("\n");
        printf("6) Publisher Enable      : %s\n", publisher_enable?"True":"False");
        printf("7) Publisher Topic       = %s\n", publisher_topic);
        printf("8) Publisher QoS         = %d\n", publisher_qos);
        printf("9) Publisher Crypto      = %s\n", publisher_crypto_mode==CRYPTO_MODE_NO_CRYPTO?"No Crypto":(publisher_crypto_mode==CRYPTO_MODE_CRYPTO_XOR?"XOR":"AES"));
        printf("A) Publisher repeat at %d ms\n",  publisher_repeat_after_ms);
        printf("B) Publisher Msg with TC : %s\n", publisher_msg_with_timetick?"True":"False");
        printf("\n");
        printf("C) Subscriber Enable     : %s\n", subscriber_enable?"True":"False");
        printf("D) Subscriber Topic      = %s\n", subscriber_topic);
        printf("E) Subscriber QoS        = %d\n", subscriber_qos);
        printf("F) Subscriber Crypto     = %s\n", subscriber_crypto_mode==CRYPTO_MODE_NO_CRYPTO?"No Crypto":(subscriber_crypto_mode==CRYPTO_MODE_CRYPTO_XOR?"XOR":"AES"));
        printf("G) Subscriber Msg with TC: %s\n", subscriber_msg_with_timetick?"True":"False");
        printf("\n");
        printf("S) Start\n");
        printf("\n\nSelecione uma opção\n\n");

        char tmp[STRING_SIZE+1];
        switch(toupper(getchar())){
            case '0': printf("Digite o novo WiFI SSID: "); read_string(wifi_ssid);  break;
            case '1': printf("Digite o novo WiFI PSW: ");  read_string(wifi_psw);   break;

            case '2': 
                printf("Digite o novo Publisher Broker IP: ");
                read_string(tmp);
                if (!ip4addr_aton(tmp, &device_brocker_ip)) {
                    printf("Erro no IP digitado: %s\n", tmp);
                    getchar();
                }
                break;
            case '3': printf("Digite o novo Device ID: ");   read_string(device_id);    break;
            case '4': printf("Digite o novo Device User: "); read_string(device_user);  break;
            case '5': printf("Digite o novo Device PSW: ");  read_string(device_psw);   break;

            case '6': publisher_enable = !publisher_enable; break;
            case '7': printf("Digite o novo Publisher Topic: "); read_string(publisher_topic); break;
            case '8':
                if((++publisher_qos) >= 3) publisher_qos = 0;
                break;
            case '9':
                if((++publisher_crypto_mode) >= CRYPTO_MODE_N_MODES) publisher_crypto_mode = 0;
                break;
            case 'A': 
                printf("Digite o novo Publisher repeat at: ");
                read_string(tmp);
                sscanf(tmp, "%d", &publisher_repeat_after_ms);
                break;
            case 'B': publisher_msg_with_timetick = !publisher_msg_with_timetick; break;

            case 'C': subscriber_enable = !subscriber_enable; break;
            case 'D': printf("Digite o novo Subscriber Topic: "); read_string(subscriber_topic); break;
            case 'E':
                if((++subscriber_qos) >= 3) subscriber_qos = 0;
                break;
            case 'F':
                if((++subscriber_crypto_mode) >= CRYPTO_MODE_N_MODES) subscriber_crypto_mode = 0;
                break;
            case 'G': subscriber_msg_with_timetick = !subscriber_msg_with_timetick; break;

            case 'S': 
                fim = true;
                break;
        }    
    }
    printf("\033[2J");
    printf("\033[H");
}
