#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "libs_ext/ssd1306.h"
#include "../include/config.h"
#include "Display.h"

#define DISPLAY_TEST_DELAY_MS 2000

// estrutura do BitDogLab-C display_oled
static uint8_t display_data[ssd1306_buffer_length];

static struct render_area display_frame_area = {
        start_column : 0,
        end_column   : ssd1306_width - 1,
        start_page   : 0,
        end_page     : ssd1306_n_pages - 1,
        buffer_length: ssd1306_width * ssd1306_n_pages      //(end_column - start_column + 1) * (end_page - start_page + 1);
    };


// Inicializa o módulo OLED
void display_init(){
    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // zera o display inteiro
    memset(display_data, 0, display_frame_area.buffer_length);
    display_update();
}

// Preenche o módulo OLED, on: true => acesso, false => apagado
void display_fill(     bool on, bool update){
    memset(display_data, on?0xFF:0x00, display_frame_area.buffer_length);
    if(update) display_update();
}

// Escreve uma mensagem no display a partir da posição cx e cy (0<=cx<=15, 0<=cy<=7)
// Obs.: o y do ssd1306_draw_string só funciona de 8 em 8
void display_msg(               bool update, uint8_t cx,  uint8_t cy , char *msg){
    ssd1306_draw_string(display_data, cx<<3, cy<<3, msg);   
    if(update) display_update();
}

// parametro on => Seta ou clear; um ponto de coordenadas> 0 <= x < 128, 0 <= y <64
void display_set_point(bool on, bool update, uint8_t x,  uint8_t y){
    ssd1306_set_pixel(display_data, x, y, on);
    if(update) display_update();
}

// parametro on => Seta ou clear; um retângulo com as coordenadas extremas: (x0,y0) e (x1, y1)
void display_set_ret(  bool on, bool update, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
    ssd1306_draw_line(display_data, x0, y0,  x1, y0, on);
    ssd1306_draw_line(display_data, x1, y0,  x1, y1, on);
    ssd1306_draw_line(display_data, x0, y1,  x1, y1, on);
    ssd1306_draw_line(display_data, x0, y0,  x0, y1, on);
    if(update) display_update();
}

// Inverte todo o conteúdo do display
void    display_invert(bool update){
    for(int i=0;i<ssd1306_buffer_length;i++) display_data[i] = ~display_data[i];
    if(update) display_update();
}

// Inverte o conteudo da posição de um caractere no display
void    display_invert_char(       bool update, uint8_t cx,  uint8_t cy){
    int pos = cy * (uint16_t)128 + cx * (uint16_t)8;
    for(int i=0;i<8;i++) display_data[i + pos] = ~display_data[i + pos];
    if(update) display_update();
}

/*
// Mostra no display dados do programa que estão definidos no arquivo Config.h
void    display_show_prog_version(){
    display_fill(false, false);
    display_msg( false,  0, 0, PROG_NAME);
    display_msg( false,  4, 2, PROG_TITLE);
    display_msg( false,  0, 4, "VERSION:");
    display_msg( false,  9, 4, PROG_VERSION);
    display_msg( false,  0, 5, "BUILD:");
    display_msg( false, 12, 5, PROG_BUILD);
    display_msg( false,  0, 7, "DATA:");
    display_msg( true,   6, 7, PROG_DATE);
}
*/
// Transfere o conteúdo da memória RAM para o Display OLED
void display_update(){
    render_on_display(display_data, &display_frame_area);
}




// Mostra no display uma imagem tipo sync
uint32_t display_sinc(bool slow);

// Faz um teste no display, mostrando textos, gráficos etc
void display_test(){
    display_init();
    printf("Display inicializado apagado\n");
    sleep_ms(DISPLAY_TEST_DELAY_MS);

    memset(display_data, 0xFF, display_frame_area.buffer_length);
    printf("Display aceso\n");
    display_update();
    sleep_ms(DISPLAY_TEST_DELAY_MS);

    ssd1306_draw_line(display_data, 0,                    0, ssd1306_width - 1, ssd1306_n_pages*8 -1, 0);
    ssd1306_draw_line(display_data, 0, ssd1306_n_pages*8 -1, ssd1306_width - 1,                    0, 0);
    printf("Display aceso com X\n");
    display_update();
    sleep_ms(DISPLAY_TEST_DELAY_MS);

    memset(display_data, 0, display_frame_area.buffer_length);
    ssd1306_draw_line(display_data, 0,                    0, ssd1306_width - 1, ssd1306_n_pages*8 -1, 1);
    ssd1306_draw_line(display_data, 0, ssd1306_n_pages*8 -1, ssd1306_width - 1,                    0, 1);

    printf("Display apagado com X\n");
    display_update();
    sleep_ms(DISPLAY_TEST_DELAY_MS);
    
    sleep_ms(100);
    display_set_ret(true, true, 0, 0, ssd1306_width  -1, ssd1306_n_pages*8 - 1);
    printf("Display apagado com X e ret\n");
    sleep_ms(DISPLAY_TEST_DELAY_MS);

    for(int i=1;i<=6;i++){
        int dx = 10 * i;
        int dy = 5 * i;
        display_set_ret(true, false, dx, dy, ssd1306_width -1 -dx, ssd1306_n_pages*8 - 1-dy);
    }
    printf("Display apagado com X e Multi Ret\n");
    display_update();
    sleep_ms(DISPLAY_TEST_DELAY_MS);

    uint32_t delay;
    printf("Display Sinc fast, Delay = ");
    delay = display_sinc(false);
    printf("%d us\n", delay);
    sleep_ms(DISPLAY_TEST_DELAY_MS);

    printf("Display Sinc slow, Delay = ");
    delay = display_sinc(true);
    printf("%d us  (%d us por ponto)\n", delay, delay / 128);
    sleep_ms(DISPLAY_TEST_DELAY_MS);

    char * msg[] = {
        "Linha 1",
        "1234567890ABCDEFGH",
        "Linha 3",
        "Linha 4",
        "Linha 5",
        "Linha 5",
        "Linha 7",
        "Linha 8",
    };
    for(int i=0;i<8;i++) ssd1306_draw_string(display_data, 0, i*8, msg[i]);
    display_update();
    printf("Linhas\n");
    sleep_ms(DISPLAY_TEST_DELAY_MS);

    display_set_ret(true, true, 0, 0, ssd1306_width  -1, ssd1306_n_pages*8 - 1);
    printf("Linhas + Bordas\n");
    sleep_ms(DISPLAY_TEST_DELAY_MS);

    memset(display_data, 0x00, display_frame_area.buffer_length);
    display_update();
    printf("Display apagado\n");
}

static const uint8_t sinc[] ={
    31,
    25,
    19,
    14,
    9,
    6,
    3,
    2,
    1,
    2,
    3,
    4,
    5,
    7,
    8,
    8,
    9,
    8,
    8,
    7,
    6,
    5,
    4,
    4,
    4,
    4,
    4,
    5,
    6,
    7,
    7,
    8,
    8,
    8,
    7,
    7,
    6,
    5,
    4,
    4,
    4,
    4,
    4,
    5,
    6,
    7,
    8,
    8,
    9,
    8,
    8,
    7,
    5,
    4,
    3,
    2,
    1,
    2,
    3,
    6,
    9,
    14,
    19,
    25,
    31,
    37,
    43,
    48,
    53,
    56,
    59,
    60,
    61,
    60,
    59,
    58,
    57,
    55,
    54,
    54,
    53,
    54,
    54,
    55,
    56,
    57,
    58,
    58,
    58,
    58,
    58,
    57,
    56,
    55,
    55,
    54,
    54,
    54,
    55,
    55,
    56,
    57,
    58,
    58,
    58,
    58,
    58,
    57,
    56,
    55,
    54,
    54,
    53,
    54,
    54,
    55,
    57,
    58,
    59,
    60,
    61,
    60,
    59,
    56,
    53,
    48,
    43,
    37
};

// Mostra no display uma imagem tipo sinc
uint32_t display_sinc(bool slow){
    memset(display_data, 0x00, display_frame_area.buffer_length);
    display_update();
    sleep_ms(100);
    uint32_t t0 = time_us_32();
    for(int i=0;i<128;i++){
        ssd1306_set_pixel(display_data, i, sinc[i], true);
        if(slow){
            display_update();
        }
    }
    uint32_t t1 = time_us_32();
    display_update();
    return t1 - t0;
}
