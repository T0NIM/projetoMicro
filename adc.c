#include "adc.h"
#include "config.h"

void adc_init(void) {
    // Configurar ADC para o canal 2
    
    // Registrador ADCON0: controla a operação do módulo A/D
    // Bits 7:6 ? ADCS1:ADCS0 = 01 ? Fosc/16
    // Bits 5-3 ? CHS2:CHS0 = 010 ? seleciona o canal AN2 (RA2)
    // Bit 2   ? GO/DONE = 0 (a conversão ainda não começou)
    // Bit 1   ? não usado
    // Bit 0   ? ADON = 1 ? liga o módulo ADC
    ADCON0 = 0b01000001;   // 0x51
    
    // Registrador ADCON1: configura os pinos da porta
    // Bit 7 ? ADFM = 1 ? justificação à direita
    // Bits 6 ? ADCS2 = 1 ? Fosc/16
    // Bits 5:4 ? não usados (considere como '0')
    // Bits 3-0 ? PCFG3:PCFG0 = 0000 (modo padrão)
    // Nesse modo padrão, os canais AN0 a AN7 são analógicos,
    // e as tensões de referência são Vref+ = VDD e Vref? = VSS
    ADCON1 = 0b11000000;  // 0xC0
   
    
}

unsigned int adc_read(unsigned char canal) {
    // Limita o canal para 0 a 7
    canal &= 0x07;

    // Atualiza apenas os bits de seleção de canal no ADCON0 (bits 5:3)
    ADCON0 &= 0b11000111;             // Limpa CHS2:CHS0
    ADCON0 |= (canal << 3);           // Ajusta para o canal desejado

    __delay_us(20);                   // Tempo de aquisição
    GO_nDONE = 1;                     // Inicia conversão

    while(GO_nDONE);                  // Aguarda fim da conversão

    return ((ADRESH << 8) | ADRESL);  // Retorna valor de 10 bits
}