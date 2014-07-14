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

#define SWITCH_MASK (1 << 2)
#define RELAY_MASK (1 << 3)


void set_light(bool);

static uint8_t measurement;

static uint8_t get(uint8_t type) {
    if (type == 1) {
        return measurement;
    }
    return 0xFF;
}

static void put(uint8_t type, uint8_t value) {
    if (type == 1) {
        status_set(value);
        set_light(value ? true : false);
    }
}

void set_light(bool on)
{
    if (on) {
        PORTA |= RELAY_MASK;
    } else {
        PORTA &= ~RELAY_MASK;
    }
    
    measurement = on;
}

int main(void)
{
    status_init();
    
    // Set relay port to out and switch port to in
    DDRA = RELAY_MASK | (DDRA & (~SWITCH_MASK));
    PORTA |= SWITCH_MASK;
    
    uart_init(1);
    twi_init(0xD);
    twi_enable_interrupt();
    twi_register_get(get);
    twi_register_put(put);
    
    // Ports PA2, PA3
    
    // Enable write on relay leg
    
    // Turn relay on
    set_light(false);
    status_set(true);
    
    sei();
    
    bool status = true;
    status_set(true);
    while (1) {
        uint8_t pressed = !(PINA & SWITCH_MASK);
        if (pressed) {
            set_light(!measurement);
            
            _delay_ms(500);
        }
        
    }
    
}
