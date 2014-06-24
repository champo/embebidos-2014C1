//
//  uart.c
//  Test
//
//  Created by Juan Pablo Civile on 22/04/14.
//
//

#include "uart.h"
#include <avr/io.h>

void uart_init(uint16_t bauds) {
    
    uint16_t ubrr = (((F_CPU) + 4UL * bauds) / (16UL * bauds) -1UL);
    ubrr = 25;
    UBRR0 = ubrr;
    
    // Enable Tx and Rx
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // 8 bits, 1 stop bits
    UCSR0C = (0 << USBS0) | (3 << UCSZ00);
}

bool uart_rx_ready(void) {
    return (UCSR0A >> RXC0) & 1;
}

uint8_t uart_poll(void) {
    while (!uart_rx_ready());
    
    return UDR0;
}

uint8_t uart_poll_echo(void) {
    uint8_t c = uart_poll();
    uart_write(c);
    return c;
}

void uart_write_byte(uint8_t value) {
    while (!(UCSR0A & (1 << UDRE0)));
    
    UDR0 = value;
}

int uart_write_string(char* buffer, int size) {
	int i;
	for(i = 0; i<size;i++){
		uart_write((uint8_t)buffer[i]);
	}
	return i;
}
