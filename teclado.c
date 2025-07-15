/*
 * File:   teclado.c
 * Author: mauro
 *
 * Created on 2 de Julho de 2025, 14:07
 */


#include "teclado.h"
#include "config.h"

// Mapeamento daas chaves para caracteres
unsigned char key_map[4][3] = {
   //[0]   [1] [2]
   // C1   C2   C3
    {'1', '2', '3'}, // L1  [0]
    {'4', '5', '6'}, // L2  [1]
    {'7', '8', '9'}, // L3  [2]
    {'*', '0', '#'}  // L4  [3]
};


unsigned char read_key(void) {
    // Varredura do teclado por linha
    // Iniciar linha com zero para compatilizar os índices com key_map
    for(unsigned char linha=0; linha < 4; linha++) {
        // Desativar todas as linhas do teclado inicialmente em cada varredura
        L1 = 1;
        L2 = 1;
        L3 = 1;
        L4 = 1;
              
        // Ativa a linha atual com zero
        if (linha == 0) L1 = 0;  // ativa linha 1 do teclado
        if (linha == 1) L2 = 0;  // ativa linha 2 do teclado
        if (linha == 2) L3 = 0;  // ativa linha 3 do teclado
        if (linha == 3) L4 = 0;  // ativa linha 4 do teclado
        
        
        atraso_ms(5); // debounce
        
        // Verifica se alguma coluna está em nível baixo
        if ((!C1 + !C2 + !C3) > 1) {
            return '\0'; // mais de uma tecla pressionada (anti-ghosting)
        }
        
        // Realiza o mapeamento    
        if (!C1) return key_map[linha][0];
        if (!C2) return key_map[linha][1];
        if (!C3) return key_map[linha][2]; 
    } // varredura
    return '\0'; // nenhuma tecla foi pressionada
}


void key_init(void) {
    TRISD &= 0xF0;  // os pinos mais baixo da porta como saída (0b1111 0000) - linhas
    TRISB |= 0x07;  // 0b0000 0111 - 3 pinos mais baixo como entrada - colunas
    
    
}
