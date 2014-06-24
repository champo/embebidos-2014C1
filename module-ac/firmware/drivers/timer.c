#include "timer.h"

#include <limits.h>

#include <avr/interrupt.h>

#define LISTENER_CANT 10

struct listener_struct {
    unsigned int count;
    function_t function;
};

typedef struct listener_struct listener;

unsigned int counter;
listener listener_list[LISTENER_CANT];

ISR(TIMER0_OVF_vect){
	counter++;
	int i;
	for(i=0;i<LISTENER_CANT;i++){
		if(counter%listener_list[i].count==0){
			listener_list[i].function();
		}
	}
}

void none(){
}

void add_listener(function_t function, unsigned int count){
	int i;
	for(i=0;i<LISTENER_CANT;i++){
		if(listener_list[i].function==none){
			listener_list[i].count=count;
			listener_list[i].function =function;
			return;
		}
	}
}

void remove_listener(function_t function){
	int i;
	for(i=0;i<LISTENER_CANT;i++){
		if(listener_list[i].function==function){
			listener_list[i].count=UINT_MAX;
			listener_list[i].function =none;
		}
	}
}

void init_timer_listener(){
	int i;
	for(i=0;i<LISTENER_CANT;i++){
		listener_list[i].count=UINT_MAX;
		listener_list[i].function =none;
	}
	
	//selecciona prescaler /1024
    TCCR0B |= (1<<CS00) | (1<<CS02);
    //enciende interrupcion de overflow
    TIMSK0 |= (1<<TOIE0);
}
