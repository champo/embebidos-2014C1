//
//  uart.h
//  Test
//
//  Created by Juan Pablo Civile on 22/04/14.
//
//

#ifndef Test_uart_h
#define Test_uart_h

#include "common.h"

void uart_init(uint16_t bauds);

bool uart_rx_ready(void);

uint8_t uart_poll(void);

uint8_t uart_poll_echo(void);

void uart_write_byte(uint8_t value);

int uart_write_string(char* buffer, int size);


#endif
