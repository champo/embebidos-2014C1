//
//  uart.c
//  Test
//
//  Created by Juan Pablo Civile on 22/04/14.
//
//

#include "buffered_uart.h"
#include <avr/io.h>


#include <avr/interrupt.h>

#define BUFFER_SIZE 256

struct buffer_struct
{
	int size;
	char * array;
	int start;
	int end;
};

typedef struct buffer_struct buffer_t;

char in_array[BUFFER_SIZE];
char out_array[BUFFER_SIZE];

buffer_t out;

buffer_t in;


int write_buffer(buffer_t* buff, char* data, int size){
	int i;
	int tmp;
	tmp=(buff->end+1)%buff->size;
	for(i=0;i<size && tmp!=buff->start;i++,tmp=(buff->end+1)%buff->size){
		buff->array[buff->end]=*((char*)(data+i));
		buff->end=tmp;
	}

	return i;
}

int read_buffer(buffer_t* buff, char* data, int size){
	int i;
	for(i=0;i<size && buff->start!=buff->end;i++){
			*((char*)(data+i))=buff->array[buff->start];
			buff->start=(buff->start+1)%buff->size;
	}
	if(i<size){
		*((char*)(data+i))='\0';
	}
	
	return i;
}

/*void uart_write_byte(uint8_t value) {
    while (!(UCSR0A & (1 << UDRE0)));
    
    UDR0 = value;
}*/


ISR(USART0_RX_vect){
	char c = UDR0;
	write_buffer(&in,&c,1);
}

ISR(USART0_UDRE_vect){
	if(out.start!=out.end){
		char c;
		read_buffer(&out,&c,1);
		UDR0 = c;
	}else{
		UCSR0B &= ~(1<<UDRIE0);
	}
}


void buffered_uart_init() {
    
    //uint16_t ubrr = (((F_CPU) + 4UL * bauds) / (16UL * bauds) -1UL);
    //ubrr = 25;
    UBRR0 = 64;
    
    // Enable Tx and Rx
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1<<RXCIE0);
    // 8 bits, 1 stop bits
    UCSR0C |= (0 << USBS0) | (3 << UCSZ00);
    
    out.size = BUFFER_SIZE;
    in.size = BUFFER_SIZE;
    out.array = out_array;
    in.array = in_array;
    in.start = 0;
    in.end = 0;
    out.start = 0;
    out.end = 0;
}


void uart_read(char* data, int size){
	int i = 0;
	while(i<size){
		i += read_buffer(&in,data+i,size-i);
	}
}

void uart_write(char* data, int size){
	int i = 0;
	while(i<size){
		i += write_buffer(&out,data+i,size-i);
		UCSR0B |= (1<<UDRIE0);
	}
}
