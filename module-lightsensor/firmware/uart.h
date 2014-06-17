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

void uart_write(uint8_t value);

void uart_send(const char* str);

#endif
