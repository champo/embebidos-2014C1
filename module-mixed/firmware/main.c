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

#define SWITCH_MASK (1 << 4)
#define RELAY_MASK (1 << 3)


void set_light(bool);

static uint8_t light = 0;
static uint8_t temperature = 0;
static uint8_t switch_toggle;


static uint8_t get(uint8_t type) {
    if (type == LUMINOSITY) {
        return light;
    } else if (type == TEMPERATURE) {
        return temperature;
    } else if (type == LIGHTS) {
        return switch_toggle;
    } else {
        return 0xFF;
    }
}

static void put(uint8_t type, uint8_t value) {
    if (type == LIGHTS) {
        status_set(value);
        set_light(value ? true : false);
    }
}

void set_light(bool on)
{
    if (on) {
        PORTD |= RELAY_MASK;
    } else {
        PORTD &= ~RELAY_MASK;
    }

    switch_toggle = on;
}

int main(void)
{
    status_init();

    // Ports PD3
    // Set relay port to out
    DDRD |= RELAY_MASK;
    
    // Port PD2, to in
    DDRD = (DDRD & (~SWITCH_MASK));
    PORTD |= SWITCH_MASK;

    adc_init();

    // Normal mode
    TCCR1A = 0;
    // Prescale by 256, ~1.25 ints per second
    TCCR1B = 0x04;
    // Enable OVF interrupt
    TIMSK1 = 1;

    uart_init(1);
    uart_send("started");
    twi_init(0xD);
    twi_enable_interrupt();
    twi_register_get(get);
    twi_register_put(put);


    // Turn relay on
    set_light(false);

    sei();

    status_set(true);
    while (1) {
        uint8_t pressed = !(PIND & SWITCH_MASK);
        if (pressed) {
            set_light(!switch_toggle);
            uart_send("Switch");

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
    adc_mode(0); // ADC0 as single
    uint16_t read = adc_read();
    temperature = read / 4;
    sprintf(m, "%dºC (raw = %d)", temperature, read);

    uart_send(m);


    adc_mode(0x12); // ADC2 - ADC1 x 1 gain = 10010
    uint16_t value = adc_read();

    uint16_t negative = value & (1 << 9);
    int16_t converted = -negative + (value & 0x1FF);
    sprintf(m, "Read = %d -- sign %d", converted, negative);
    uart_send(m);

    light = (converted + 512) / 10;
}
