/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "status.h"
#include "adc.h"
#include "uart.h"
#include "twi.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

uint8_t measurement;

uint8_t get(void) {
    return measurement;
}

int main(void)
{
    status_init();
    
    uart_init(0);
    uart_send("Ready!");
    
    twi_init(0xB);
    twi_register_get(get);
    twi_enable_interrupt();
    
    adc_init();
    
    sei();
    
    char m[40];
    while (1) {
        uint16_t value = adc_read();
        int16_t converted = (-(value & (1 << 9))) | (value & (~(1 << 9)));
        sprintf(m, "Read = %d", converted + 512);
        uart_send(m);
        
        measurement = (converted + 512) / 10;
        
        _delay_ms(500);
    }
}
