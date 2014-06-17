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


static uint8_t measurement;

static uint8_t get(void) {
    return 0x1;
    //return measurement;
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
    
    while (1) {
        
        // VRef is 2.56V
        // at T = 0, VOut = 0.5V. VOut scales at 10mV / ºC
        // T = ((adc * (2.56 / 1024)) - 0.5) * 100
        // T = adc * 256 / 1024 - 50
        // T = adc / 4 - 50
        measurement = (adc_read() / 4) - 50;
        sprintf(str, "%dºC", measurement);
        
        uart_send(str);
        _delay_ms(500);
    }
    
}
