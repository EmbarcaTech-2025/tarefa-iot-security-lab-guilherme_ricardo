/**
 * @file    cipher.c
 * @author  Guilherme Alves dos Santos e Ricardo Jorge Furlan
 * @brief  
 * @version 0.1
 * @date    2025-06-04
 * 
 */
#include <string.h>
#include "../include/cipher.h"
#include "pico/cyw43_arch.h"  


/**
 * @brief Cifra uma sequência de bytes aplicando XOR da key em cada byte
 * 
 * @param input,  ponteiro da sequência de bytes que será cifrada;
 * @param output, ponteiro da sequência de bytes a onde a sequência de bytes que será colocada;
 * @param len,    número de bytes da sequência de bytes;
 * @param key,    chave, byte que será utilizado na operação de XOr 
 */
void cipher_xor(const uint8_t *input, uint8_t *output, size_t len, uint8_t key){
    for(int i=0;i<len;i++){
        output[i] = input[i] ^ key;
    }
}

/**
 * @brief Gera uma mensagem do tipo: {"value":valor,"tc":valor_do_tc}"
 * 
 * @param msg,   ponteiro do destino da mensagem será gravada, tem que ser grande o suficiente para caber a mensagem
 * @param tc,    timetick, valor que indica o número da mensagem,
 * @param value, valor/dado da mensagem 
 */
void cipher_msg_generate(char *msg, int32_t tc, int32_t value){
    sprintf(msg, "{\"value\":%d,\"tc\":%d}", value, tc);
}

/**
 * @brief A partir de um string com uma mensagem, extrai: tc e value
 * 
 * se conseguir extrair a informação retorna true, senão false
 * 
 * @param msg,   string com a mensagem que será "decodificada" 
 * @param tc,    ponteiro a onde será escrito o tc contido na mensagem 
 * @param value, ponteiro a onde será escrito o valor contido na mensagem
 * @return true,      Conseguiu extrair da mensagem tc e value 
 * @return false, Não Conseguiu extrair da mensagem tc e value 
 */
bool cipher_msg_get(char *msg, int32_t *tc, int32_t *value){
    return sscanf(msg, "{\"value\":%d,\"tc\":%d}", value, tc) == 2;
}