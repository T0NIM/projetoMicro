/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2015  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

/* #########################################################################
 * Por Mauro Hemerly (Hämmerli) Gazzani
 * Adaptado para controlar o LCD em modo de 4 bits para usar juntamente com o
 * teclado matricial que ocupa os pinos de RD3-RD0 para as suas linhas, 
 * enquanto o LCD fica com os pinos restantes, RD7-RD4.
 * Para usar o LCD em modo 4 bits é necessário configurar para este modo,
 * e quando do envio de 1 byte de comando ou código de caracter, deve-se
 * enviar primeiro o nibble mais significativo, aguardar a estabilização
 * do LCD e, em seguida, enviar o nibble menos significativo. 
 * ######################################################################### */

#include <xc.h>
#include "config.h"
#include "lcd_4bits.h"

// Envia 4 bits para o barramento de dados do LCD
void lcd_wr_nibble(unsigned char nibble) {
    // Os pinos D4-D7 estão nos bits 4-7, conforme documentação da placa PICGENIOS
//    RD7 = (nibble >> 7) & 0x01;  // bit mais significativo do nibble
//    RD6 = (nibble >> 6) & 0x01;
//    RD5 = (nibble >> 5) & 0x01;
//    RD4 = (nibble >> 4) & 0x01;
    
    // Versão em um único comando 
    LPORT = (LPORT & 0x0F) | (nibble & 0xF0);
    
    LENA = 1;
    atraso_us(1);
    LENA = 0;
    atraso_us(100);
}

// Envia um comando de 8 bits (dividido em dois nibbles)
void lcd_cmd(unsigned char val) {
    LDAT = 0;
    lcd_wr_nibble(val);         // envia os 4 MSB
    lcd_wr_nibble((unsigned char)(val << 4));    // envia os 4 LSB
}

// Envia um dado (caractere) de 8 bits (dividido em dois nibbles)
void lcd_dat(unsigned char val) {
    LDAT = 1;
    lcd_wr_nibble(val);         // envia os 4 MSB
    lcd_wr_nibble((unsigned char)(val << 4));    // envia os 4 LSB
}

// Inicializa o LCD no modo 4 bits
void lcd_init(void) {
    LENA = 0;
    LDAT = 0;
    atraso_ms(20); // espera inicial

    // Sequência especial de inicialização para 4 bits
    lcd_wr_nibble(0x30); // modo 8 bits (repetido 3 vezes)
    atraso_ms(5);
    lcd_wr_nibble(0x30);
    atraso_ms(1);
    lcd_wr_nibble(0x30);
    atraso_ms(1);
    lcd_wr_nibble(0x20); // muda para modo 4 bits
    atraso_ms(1);

    // Agora pode usar comandos de 8 bits divididos em 2 nibbles
    lcd_cmd(0x28); // interface 4 bits, 2 linhas, fonte 5x8
    lcd_cmd(0x08); // display off
    lcd_cmd(0x0C); // display on, cursor off, blink off
    lcd_cmd(0x06); // incremento automático, sem shift
    lcd_cmd(0x01); // limpa display
    atraso_ms(2);
    lcd_cmd(0x80); // posição inicial (linha 1, coluna 0)
}

// Envia uma string para o LCD
void lcd_str(const char* str) {
    while (*str != '\0') {
        lcd_dat(*str++);
    }
}
