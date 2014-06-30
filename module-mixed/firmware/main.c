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

#define SWITCH_MASK (1 << 6)
#define RELAY_MASK (1 << 7)


void set_light(bool);

static uint8_t light = 0;
static uint8_t temperature = 0;
static uint8_t switch_toggle;

static uint8_t get(void) {
    return temperature;
}

static void put(uint8_t value) {
    status_set(value);
    set_light(value ? true : false);
}

void set_light(bool on)
{
    if (on) {
        PORTA |= RELAY_MASK;
    } else {
        PORTA &= ~RELAY_MASK;
    }

    switch_toggle = on;
}

int main(void)
{
    status_init();

    // Ports PA6, PA7
    // Set relay port to out and switch port to in
    DDRA = RELAY_MASK | (DDRA & (~SWITCH_MASK));
    PORTA |= SWITCH_MASK;
    
    // Normal mode
    TCCR1A = 0;
    // Prescale by 256, ~1.25 ints per second
    TCCR1B = 0x04;
    // Enable OVF interrupt
    TIMSK1 = 1;

    uart_init(1);
    twi_init(0xD);
    twi_enable_interrupt();
    twi_register_get(get);
    twi_register_put(put);

    // Turn relay on
    set_light(false);
    status_set(true);

    sei();

    status_set(true);
    while (1) {
        uint8_t pressed = !(PINA & SWITCH_MASK);
        if (pressed) {
            set_light(!switch_toggle);

            _delay_ms(300);
        }

    }

}

ISR(TIMER1_OVF_vect) {
    char m[40];
    
    // VRef is 2.56V
    // at T = 0, VOut = 0V. VOut scales at 10mV / ºC
    // T = adc * (2.56 / 1024) * 100
    // T = adc * 256 / 1024
    // T = adc / 4
    adc_init(2); // ADC2 as single
    uint16_t read = adc_read();
    temperature = read / 4;
    sprintf(m, "%dºC (raw = %d)", temperature, read);
    
    uart_send(m);
    
    adc_init(1 << 4); // ADC0 - ADC1 x 1 gain
    uint16_t value = adc_read();
    int16_t converted = (-(value & (1 << 9))) | (value & (~(1 << 9)));
    sprintf(m, "Read = %d", converted + 512);
    uart_send(m);
    
    light = (converted + 512) / 10;
}
