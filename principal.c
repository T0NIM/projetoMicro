#include "config.h"
#include "adc.h"
#include "lcd_4bits.h"
#include "float_str.h"
#include "teclado.h"

void init_PWM(void) {
    TRISC2 = 0;          // RC2 (CCP1) como saída
    PR2 = 255;           // Define período PWM (5kHz para Fosc = 8MHz e prescaler 4)
    CCP1CON = 0b00001100; // Modo PWM
    T2CON = 0b00000101;   // Timer2 ligado, prescaler 4
    // Duty cycle inicial (0%)
    CCPR1L = 0;
    CCP1CON &= 0xCF; // Zera bits DC1B1:DC1B0
    TMR2 = 0;          // Zera Timer2
    T2CONbits.TMR2ON = 1;  // Liga Timer2
}
void set_duty(unsigned int duty) {
    if (duty > 1023) duty = 1023;
    CCPR1L = duty >> 2;  
    CCP1CON = (CCP1CON & 0xCF) | ((duty & 0x03) << 4);  
}

float str_to_float(const char *str) {
    unsigned int res = 0;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return (float)res;
}

const char digits[10] = { // Tabela que decodifica os segmentos dos displays em digitos compreensíveis
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

int main(void) {
    unsigned char modo = 0, modo_flag = 1, heater_flag = 0, cooler_flag = 0, heater_on = 0, fan_on = 0, submodo_auto = 0;
    unsigned char int_pisca = 0, key;
    unsigned int lm35_read, setpoint_temp_read, int_teclado = 0;
    char entrada_teclado[5] = ""; // até 3 dígitos + null + margem
    unsigned char index = 0;
    float temp, setpoint_temp, histerese = 5.0;
    char temp_str[8], setpoint_temp_str[8];
    TRISC5 = 0;      // Heater
    TRISA = 0b00000111;    // Entradas analógicas
    TRISB = 0b00011111; // RB0-RB2 entrada
    PORTB = 0x00;
    TRISC1 = 0;     // Buzzer
    TRISE &= 0xF9;  // Pinos RE1 e RE2 como saída
    TRISD &= 0x0F;  // RD4-RD7 como saída
    TRISC2 = 0;     // CCP1 PWM Fan
    TRISC1 = 0;
    adc_init();
    init_PWM();
    lcd_init();
    lcd_cmd(L_NCR);
    
    while (1) {
        // Leitura do sensor e trimpot
        lm35_read = adc_read(2);
        setpoint_temp_read = adc_read(0);
        temp = (float)(lm35_read * 500.0 / 1023.0);
        setpoint_temp = (float)(setpoint_temp_read / 10);
        
      
        // Alternar entre modo manual e automático
        if (RB2 == 0) {
            lcd_cmd(L_NCR);
            __delay_ms(50);
            while (RB2 == 0);
            modo = !modo;
            modo_flag = !modo_flag;
            submodo_auto = 0; // Reseta submodo teclado
            PORTCbits.RC5 = 0;
            set_duty(0);
            heater_flag = 0;
            cooler_flag = 0;
            PORTBbits.RB6 = 0;
            PORTBbits.RB7 = 0;
            lcd_cmd(L_CLR);
        }
        
        
        // Alternar submodo TECLADO dentro do automático
        if (modo == 1 && RB1 == 0) {
            __delay_ms(50);
            while (RB1 == 0);
            submodo_auto = !submodo_auto;
            lcd_cmd(L_NCR);
            if (submodo_auto) {
                PORTCbits.RC5 = 0;
                set_duty(0);
                PORTBbits.RB6 = 0;
                PORTBbits.RB7 = 0;
                lcd_cmd(L_CLR);
                lcd_cmd(L_ON);
                lcd_cmd(L_L1);
                print_lcd("  Modo Teclado");
                lcd_cmd(L_CLR);
                lcd_cmd(L_L1);
                print_lcd("  Modo Teclado");
                lcd_cmd(L_L2);
                print_lcd("Setpoint: ");
                key_init();  // Inicializa teclado
                index = 0;
                for (unsigned char i = 0; i < sizeof(entrada_teclado); i++) {
                    entrada_teclado[i] = '\0';
                }
                while (1) {
                    key = read_key();
                    if (key >= '0' && key <= '9') {
                        if (index < 3) {
                            entrada_teclado[index++] = key;
                            entrada_teclado[index] = '\0'; // Null-terminate
                            lcd_cmd(L_L2);
                            print_lcd("Setpoint: ");
                            print_lcd(entrada_teclado);
                        }
                        while (read_key() != '\0'); // Espera soltar a tecla
                    }
                    // Apagar último caractere com '*'
                    if (key == '*') {
                        if (index > 0) {
                            entrada_teclado[--index] = '\0';
                            lcd_cmd(L_L2);
                            print_lcd("Setpoint:     "); // Limpa linha
                            lcd_cmd(L_L2);
                            print_lcd("Setpoint: ");
                            print_lcd(entrada_teclado);
                        }
                        while (read_key() != '\0');
                    }
                    // Confirmar com RB3
                    if (RB3 == 0 && index > 0) {
                        __delay_ms(50);
                        while (RB3 == 0);
                        int_teclado = 1;
                        lcd_cmd(L_CLR);
                        lcd_cmd(L_NCR);
                        modo = 1;
                        submodo_auto = 0;
                        break;
                    }
                }
            }
        }
        if (modo == 0) {
            // --- MODO MANUAL ---
            // Heater manual ON/OFF
            if (RB1 == 0) {
                __delay_ms(50);
                while (RB1 == 0);
                heater_flag = !heater_flag;
                PORTCbits.RC5 = heater_flag;
                PORTBbits.RB6 = heater_flag;
            }
            // Fan manual ON/OFF
            if (RB0 == 0) {
                __delay_ms(50);
                while (RB0 == 0);
                cooler_flag = !cooler_flag;
                set_duty(cooler_flag ? 1024 : 0);
                if (!cooler_flag) PORTBbits.RB7 = 0;
            }
            // Piscar LED RB7 se Fan ON
            if (cooler_flag) {
                int_pisca++;
                if (int_pisca >= 4) {
                    PORTBbits.RB7 = !PORTBbits.RB7;
                    int_pisca = 0;
                }
            } else {
                PORTBbits.RB7 = 0;
            }
            // LCD manual
            lcd_cmd(L_L1);
            print_lcd("Modo Manual");
            lcd_cmd(L_L2);
            print_lcd("Temp: ");
            float_str(temp, temp_str);
            print_lcd(temp_str);
            lcd_dat(0xDF);
            print_lcd("C");
            lcd_cmd(L_L3);
            print_lcd(heater_flag ? "Esquentando" : "           ");
            lcd_cmd(L_L4);
            print_lcd(cooler_flag ? "Esfriando" : "         ");
        }
        else if (modo == 1 && submodo_auto == 0) {
            // --- MODO AUTOMÁTICO NORMAL ---
            if(int_teclado == 1){
                setpoint_temp = str_to_float(entrada_teclado);
            }    
            
            // Lógica heater/fan
            if (temp <= setpoint_temp - histerese) heater_on = 1;
            if (temp >= setpoint_temp) heater_on = 0;
            if (temp >= setpoint_temp + histerese) fan_on = 1;
            if (temp <= setpoint_temp) fan_on = 0;
            PORTCbits.RC5 = heater_on;
            PORTBbits.RB6 = heater_on;
            set_duty(fan_on ? 1024 : 0);
            // Piscar LED RB7 se fan ativo
            if (fan_on) {
                int_pisca++;
                if (int_pisca >= 4) {
                    PORTBbits.RB7 = !PORTBbits.RB7;
                    int_pisca = 0;
                }
            } else {
                PORTBbits.RB7 = 0;
            }
            // LCD automático
            lcd_cmd(L_L1);
            if(int_teclado == 1){
                print_lcd("  Modo Teclado");
            }
            else{
                print_lcd("  Modo Trimpot");
            }
            lcd_cmd(L_L2);
            print_lcd("Atual: ");
            float_str(temp, temp_str);
            print_lcd(temp_str);
            lcd_dat(0xDF);
            print_lcd("C");
            lcd_cmd(L_L3);
            print_lcd("Alvo: ");
            float_str(setpoint_temp, setpoint_temp_str);
            print_lcd(setpoint_temp_str);
            lcd_dat(0xDF);
            print_lcd("C");
            lcd_cmd(L_L4);
            if (heater_on == 1){
                print_lcd("Esquentando");
            }
            else if (fan_on == 1){
                print_lcd("Esfriando  ");
            }
            else if (heater_on == 0 && fan_on == 0){
                print_lcd("Estavel      ");
            }
                         
            if(RB0 == 0){
                int_teclado = 0;
            }
        }
    }
}