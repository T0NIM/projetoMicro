#include "float_str.h"

// Converte ponto flutuante em string no seguinte formato: +/-999,99
void float_str(float valor, char *str) {
    unsigned int inteiro;
    unsigned int negativo = 0;

    if (valor < 0) {
        negativo = 1;
        valor = -valor;
    }

    // Multiplica por 100 e arredonda
    unsigned int num = (unsigned int)(valor * 100 + 0.5);

    inteiro = (unsigned int)(num / 100);
    unsigned int decimal = num % 100;

    // Constrói a string manualmente
    int pos = 0;

    if (negativo) {
        str[pos++] = '-';
    }
    else {
        str[pos++] = '+';
    }

    // Parte inteira (até 3 dígitos)
    if (inteiro >= 100) {
        str[pos++] = (inteiro / 100) + '0';
        str[pos++] = ((inteiro / 10) % 10) + '0';
        str[pos++] = (inteiro % 10) + '0';
    } else if (inteiro >= 10) {
        str[pos++] = (inteiro / 10) + '0';
        str[pos++] = (inteiro % 10) + '0';
    } else {
        str[pos++] = inteiro + '0';
    }

    str[pos++] = ',';

    // Parte decimal (dois dígitos sempre)
    str[pos++] = (decimal / 10) + '0';
    str[pos++] = (decimal % 10) + '0';

    str[pos] = '\0'; // final da string
}

