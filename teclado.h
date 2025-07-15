
#ifndef TECLADO_H
#define	TECLADO_H

#include <xc.h> // include processor files - each processor file is guarded.  

// Linhas do Teclado 
// Diferente da documentação da placa, talvez tenha sido invertido na programação do emulador
#define L1 RD3
#define L2 RD2
#define L3 RD1
#define L4 RD0

// Colunas do Teclado
#define C1 RB0
#define C2 RB1
#define C3 RB2

unsigned char read_key(void); 
void key_init(void);


#endif	

