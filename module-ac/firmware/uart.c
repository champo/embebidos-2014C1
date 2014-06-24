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
    
    //uint16_t ubrr = (((F_CPU) + 4UL * bauds) / (16UL * bauds) -1UL);
    // 9600 bauds @ 20 MHz
    UBRR0 = 129;
    
    // Enable Tx and Rx
    UCSR0B = (1 << RXEN0)| (1 << TXEN0);
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

void uart_write(uint8_t value) {
    while (!(UCSR0A & (1 << UDRE0)));
    
    UDR0 = value;
}

void uart_send(const char* str) {
    
    
    int i = 0;
    while (str[i]) {
        uart_write(str[i]);
        i++;
    }
    uart_write('\n');
    uart_write('\r');
}
