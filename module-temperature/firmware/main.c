/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "common.h"
#include "status.h"
#include "twi.h"
#include "uart.h"

static bool state = true;

static uint8_t get(void) {
    state = !state;
    
    status_set(state);
    return 1;
}

int main(void)
{
    status_init();
    status_set(false);
    
    uart_init(1);
    
    twi_init(0x2);
    twi_register_get(get);
    sei();
    while (1);
    
    while (1) {
        
        uint8_t value = 0;
        uart_write('1');
        if (twi_get(0x2, &value) == true && value == 1) {
            status_set(true);
            _delay_ms(1000);
        } else {
            uart_write('2');
            
            status_set(true);
            _delay_ms(100);
            status_set(false);
            _delay_ms(100);
            status_set(true);
            _delay_ms(100);
            status_set(false);
            _delay_ms(100);
            status_set(true);
            _delay_ms(100);
            status_set(false);
            _delay_ms(100);
            status_set(true);
            _delay_ms(100);
            status_set(false);
            _delay_ms(100);
            status_set(true);
            _delay_ms(100);
            status_set(false);
            _delay_ms(100);
        }
        
    }
}
