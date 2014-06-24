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
/*
 
 
 ISR(ADC_vect){
	
	//read adt
    uint8_t c = ADCH;
    
    uart_write('-');
    uart_write((c&(1<<7))?'1':'0');
    uart_write((c&(1<<6))?'1':'0');
    uart_write((c&(1<<5))?'1':'0');
    uart_write((c&(1<<4))?'1':'0');
    uart_write((c&(1<<3))?'1':'0');
    uart_write((c&(1<<2))?'1':'0');
    uart_write((c&(1<<1))?'1':'0');
    uart_write((c&(1<<0))?'1':'0');
}
*/

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


//ISR(PCINT1_vect){
////    PORTD ^= (0x80);
//}
/*
ISR(PCINT0_vect){

    if(!init){
        //TODO : iniciar timer
        TCNT1 = 0;
        init = true;
        TIMSK1 |= (1<<TOIE1);
    }
        times[pos++] = TCNT1;

    //TODO setterar el valor en times[index++] = tiempo
    
    
    
//    TCCR1B ^= (1<<ICES1);


     if(PINA & 1){
//         PORTD &= (~0x80);
//         uart_write("1\t",2);
//         PORTD ^= (0x80);
//         PORTA = 2;
     }else{
//        uart_write("0\t",2);
//            PORTD |= (0x80);
//         PORTA = 0;
        }

}

ISR(TIMER1_OVF_vect){
    overflow_done = true;
    PCICR = (0<<PCIE0);
    PCMSK0 = (0<<PCINT0);
//    char buffer[12];
//    sprintf(buffer, "pos = %d\r\n",pos);
//    uart_write(buffer, strlen(buffer));
    if(!is_written){
        is_written = true;
//        eeprom_busy_wait();
//        eeprom_write_block (times, signal1, EESIZE*2);
    }

}
*/
int main(void)
{
    CLKPR = 0;
    
    //encender led
    DDRD = 0x80;
//    DDRD |= 1<<7;
    DDRA = 0;
//     PORTD |= (0x80);


//	init_timer_listener();
	
//    buffered_uart_init(19200);
    
    //TIMER
	//selecciona prescaler /1024
    //    TCCR0B |= (1<<CS00) | (1<<CS02);
    //selecciona prescaler /1024
    //    TCCR1A |= (1<COM1A1) | (0<COM1A0);
    //prescaler en 1024
    TCCR1B |= (1<<CS12) | (0<<CS11) | (1<<CS10);
    //CTC para ICRn (para que reinicie el counter al matchear
//    TCCR1B |= (1<<WGM13) | (1<<WGM12);
    // Iinput Capture Edge Select en asc
//    TCCR1B |= (1<<ICES1) | (1<<ICNC1);
    //configuro para que el numero a comparar sea 20000000/1024/5 = 2*1953 = 3906 = 0x7A1
    //    OCR1A = 0xF42;
    //enciende interrupcion de overflow
    
    //    TIMSK1 |= (1<<TOIE1);
    // Activo la interupcion por input capture
//    TIMSK1 |= (1<<ICIE1);
//    PCICR = (1<<PCIE0);
//    PCMSK0 = (1<<PCINT0);// | (1<<PCINT1);
    
    
    //COM2A1:0 en 1 y 0: Clear OC2A on Compare Match when up-counting. Set OC2A on Compare Match when down-counting.
    //COM2A1:0 en 1 y 1: Set OC2A on Compare Match when up-counting. Clear OC2A on Compare Match when down-counting.
   //Configuro timer 2 como correct phase pwm
    TCCR2A |= (1<<WGM20) | (0<<COM2A1) | (1<<COM2A0);
    TCCR2B |= (1<<WGM22);
    //lo desactivo
    TCCR2B &= ~((1<<CS12) | (1<<CS11) | (1 <<CS10));
    OCR2A = 16;
    
    //lo activo
//    TCCR2B |= (0<<CS12) | (1<<CS11) | (0 <<CS10);
//    
    twi_init(0x0F);
//
    twi_register_get(get);
//
    twi_register_put(put);
//
    twi_enable_interrupt();
    /*
    
    //ADC
    //internal voltage ref 2.56V
    ADMUX |= (1<<REFS0) | (1<< ADLAR);
    //timer preescaler 8 000 000 / 128 = 62 000
    ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    //init adc
    ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (1<<ADIE);
    //cada cuanto tiempo
    ADCSRB |= (1<<ADTS2);
   
   */
    
    //activa interrupts
    sei();
// 	while(true);
 
// 	shell_start();

    while(1){
//        int i;
        
//        if(on){
//        eeprom_busy_wait();
//        eeprom_read_block (sig1, signal2, EESIZE*2);
//            on = false;
//        eeprom_busy_wait();
//            
//        }else{
//            eeprom_busy_wait();
//            eeprom_read_block (sig1, signal1, EESIZE*2);
//            on = true;
//            eeprom_busy_wait();
//        }
//        int aux = 0;
//        while(!overflow_done);
        
//        overflow_done = false;
//        _delay_ms(1500);
//        aux = on?1:0;
//        put(aux);
        /*
        TCNT1 = 0;
        for(i=0;i<228;i++){ //volver a pos
            
//            char buffer[20];
//            sprintf(buffer, "TCNT1 = %d - times = %d, i = %d\r\n",TCNT1,sig1[i],i);
//            uart_write(buffer, strlen(buffer));
            while(sig1[i] > TCNT1);
//            PORTD ^= (0x80);
            aux ^= 1;
//            PORTA = (aux<<1);
            if(aux>0){
                //lo activo
//                TCNT2 = 0;
                TCCR2B |= (0<<CS12) | (1<<CS11) | (0 <<CS10);
            }else{
                //lo desactivo
                TCCR2B &= ~((1<<CS12) | (1<<CS11) | (1 <<CS10));
//                PORTD |= (0x80);
            }

        }
        //lo desactivo
        TCCR2B &= ~((1<<CS12) | (1<<CS11) | (1 <<CS10));
        PORTD |= (0x80);
   
*/
        
//        if(TIFR1 & (1<<ICF1)){
//            PORTD &= (~0x80);
//        }else{
//            PORTD |= (0x80);
//        }
//        if(PINA & 1){
//            uart_write("1\t",2);
//            //         PORTD ^= (0x80);
//        }else{
//            uart_write("0\t",2);
//            //            PORTD |= (0x80);
//        }

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
