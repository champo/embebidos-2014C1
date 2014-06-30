//
//  adc.c
//  CrumbTest
//
//  Created by Juan Pablo Civile on 06/05/14.
//
//

#include "adc.h"

#include <avr/io.h>
#include <util/delay.h>

#include "common.h"

void adc_init(void) {
    // ADC enable, single mode, 128 divisor from CLK
    ADCSRA = BIT_MASK(1, 0, 0, 0, 0, 1, 1, 1);
    
    ADMUX = (1 << REFS1) | (1 << REFS0) | 0;
}

void adc_mode(uint8_t mux) {
    
    // Wait till current conversion ends just in case
    while (ADCSRA & (1 << ADSC));
    _delay_ms(1);
    
    // Using internal 2.56V, mux as input
    ADMUX = (1 << REFS1) | (1 << REFS0) | (mux & 0x1F);
    
    // Needed to stabilize channel changes
    _delay_us(500);
}

uint16_t adc_read(void) {
    
    ADCSRA |= 1 << ADSC;
    
    while (!(ADCSRA & (1 << ADIF)));
    
    uint8_t low = ADCL;
    return (ADCH << 8) | low;
}