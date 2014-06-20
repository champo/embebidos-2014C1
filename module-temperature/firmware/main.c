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


volatile static uint8_t measurement;

static uint8_t get(void) {
    return measurement;
}

int main(void)
{
    char str[30];
    status_init();
    adc_init();
    
    status_set(false);
    
    uart_init(1);
    twi_init(0xC);
    twi_enable_interrupt();
    twi_register_get(get);
    
    measurement = 0;
    
    sei();
    
    while (1) {
        
        // VRef is 2.56V
        // at T = 0, VOut = 0V. VOut scales at 10mV / ºC
        // T = adc * (2.56 / 1024) * 100
        // T = adc * 256 / 1024
        // T = adc / 4
        uint16_t read = adc_read();
        measurement = read / 4;
        sprintf(str, "%dºC (raw = %d)", measurement, read);
        
        uart_send(str);
        _delay_ms(500);
    }
    
}
