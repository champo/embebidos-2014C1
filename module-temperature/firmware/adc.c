//
//  adc.c
//  CrumbTest
//
//  Created by Juan Pablo Civile on 06/05/14.
//
//

#include "adc.h"

#include <avr/io.h>
#include "common.h"

void adc_init(void) {
    // Using internal 2.56V, ADC0 as single input
    ADMUX = BIT_MASK(1, 1, 0, 0, 0, 0, 0, 1);
    
    // ADC enable, single mode, 16 divisor from CLK
    ADCSRA = BIT_MASK(1, 0, 0, 0, 0, 1, 1, 1);
}

uint16_t adc_read(void) {
    
    ADCSRA |= 1 << ADSC;
    
    while (!(ADCSRA & (1 << ADIF)));
    
    uint8_t low = ADCL;
    return (ADCH << 8) | low;
}