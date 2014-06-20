/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "common.h"
#include "status.h"
#include "twi.h"
#include "uart.h"
#include "adc.h"

#include <avr/io.h>
#include "common.h"

#define SWITCH_MASK PC2
#define RELAY_MASK (1 << 3)


static uint8_t measurement;

static uint8_t get(void) {
    return measurement;
}

int main(void)
{
    status_init();
    adc_init();
    
    
    uart_init(1);
    twi_init(0xD);
    twi_enable_interrupt();
    twi_register_get(get);
    
    measurement = 0;
    
    // Ports PC2, PC3
    
    _delay_ms(300);
    
    // Enable write on relay leg
    DDRC |= RELAY_MASK;
    
    // Turn relay on
    PORTC |= RELAY_MASK;
    
    for (measurement = 0; measurement < 10; measurement++);
    
    //PORTC = (PORTC & ~RELAY_MASK);
    
    status_set(true);
    
    //PORTD = ((~status) << 7) | (PORTD & ~STATUS_MASK);
    
    while (1) {
        
        
    }
    
}
