#ifndef DISPLAY_DEF
#define DISPLAY_DEF

// Obs.: O parâmetro update é utilizada para fazer a transferência da imagem do OLED que está em RAM para o dispositivo
//       como muitas vezes são feitas varias operações antes do necessário update, este parâmetro se torna útil

// Inicializa o módulo OLED
void    display_init();

// Preenche o módulo OLED, on: true => acesso, false => apagado
void    display_fill(     bool on, bool update);

// parametro on => Seta ou clear; um ponto de coordenadas> 0 <= x < 128, 0 <= y <64
void    display_set_point(bool on, bool update, uint8_t x,  uint8_t y);

// parametro on => Seta ou clear; um retângulo com as coordenadas extremas: (x0,y0) e (x1, y1)
void    display_set_ret(  bool on, bool update, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

// Escreve uma mensagem no display a partir da posição cx e cy (0<=cx<=15, 0<=cy<=7)
void    display_msg(               bool update, uint8_t cx,  uint8_t cy , char *msg);

// Inverte todo o conteúdo do display
void    display_invert(            bool update);

// Inverte o conteudo da posição de um caractere no display
void    display_invert_char(       bool update, uint8_t cx,  uint8_t cy);

// Mostra no display dados do programa que estão definidos no arquivo Config.h
void    display_show_prog_version();

// Transfere o conteúdo da memória RAM para o Display OLED
void    display_update();

// Faz um teste no display, mostrando textos, gráficos etc
void    display_test();

#endif
