#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define TRIMPOT1 TRISA0

void adc_init(void);
unsigned int adc_read(unsigned char canal);

#endif	
