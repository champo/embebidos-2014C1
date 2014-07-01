/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#define F_CPU 20000000UL  // 20 MHz
//#define F_CPU 14.7456E6
#include <util/delay.h>
#include "string.h"
#include "common.h"
#include "twi.h"

//#include "drivers/buffered_uart.h"
//#include "drivers/timer.h"

//#include "shell/shell.h"
#include <stdlib.h>
#include "stdio.h"

#include <avr/io.h>

#include <avr/interrupt.h>

#include <avr/eeprom.h>

#define EESIZE 256

uint8_t get();
void put(uint8_t in);

bool init = false;
volatile bool overflow_done = false;
volatile bool on = false;
//volatile int times[EESIZE];
volatile int pos = 0;
volatile bool is_written = false;
int sig1[EESIZE*2];
int signal1[EESIZE] EEMEM; //off
int signal2[EESIZE] EEMEM; //on 24


int main(void)
{
    CLKPR = 0;

    //encender led
    DDRD = 0x80;
//    DDRD |= 1<<7;
    DDRA = 0;
//     PORTD |= (0x80);

    //prescaler en 1024
    TCCR1B |= (1<<CS12) | (0<<CS11) | (1<<CS10);


    TCCR2A |= (1<<WGM20) | (0<<COM2A1) | (1<<COM2A0);
    TCCR2B |= (1<<WGM22);
    //lo desactivo
    TCCR2B &= ~((1<<CS12) | (1<<CS11) | (1 <<CS10));
    OCR2A = 16;

    twi_init(0x0F);
    twi_register_get(get);
    twi_register_put(put);
    twi_enable_interrupt();

    //activa interrupts
    sei();

    while(1){
    }

    return 0;
}

uint8_t get(){
    return !on;
}

void put(uint8_t in){

    if(in>0){
        eeprom_busy_wait();
        eeprom_read_block (sig1, signal2, EESIZE*2);
        on = false;
        eeprom_busy_wait();

    }else{
        eeprom_busy_wait();
        eeprom_read_block (sig1, signal1, EESIZE*2);
        on = true;
        eeprom_busy_wait();
    }
    int i;
    int aux = 0;
    TCNT1 = 0;
    for(i=0;i<228;i++){
        while(sig1[i] > TCNT1);
        aux ^= 1;
        if(aux>0){
            //lo activo
            TCCR2B |= (0<<CS12) | (1<<CS11) | (0 <<CS10);
        }else{
            //lo desactivo
            TCCR2B &= ~((1<<CS12) | (1<<CS11) | (1 <<CS10));
            PORTD |= (0x80);
        }

    }
    //lo desactivo
    TCCR2B &= ~((1<<CS12) | (1<<CS11) | (1 <<CS10));
    PORTD |= (0x80);

}
