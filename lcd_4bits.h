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

#define LENA  PORTEbits.RE1   // Enable (E)
// Seleção: comando(RS=0) ou dado (RS=1)
#define LDAT  PORTEbits.RE2   // RS
#define LPORT PORTD  // Porta de "dados" (DB7-DB0)


#define L_ON	0x0F  // habilita LCD
#define L_OFF	0x08  // desabilita LCD
#define L_CLR	0x01  // limpa LCD
#define L_L1	0x80  // posiciona cursor na primeira linha
#define L_L2	0xC0  // posiciona cursor na segunda linha
#define L_CR	0x0F  // cursor visível		
#define L_NCR	0x0C  // cursor invisível

#define L_CFG   0x38  // configura LCD: 8-bit, 2 linhas, 5*7 pixels

#define print_lcd   lcd_str

#define L_L3  0x90  // Terceira linha (início em 0x10)
#define L_L4  0xD0  // Quarta linha (início em 0x50)

void lcd_init(void);
void lcd_cmd(unsigned char val); 
void lcd_dat(unsigned char val);
void lcd_str(const char* str);
