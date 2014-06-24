#ifndef __IO_H__

#define __IO_H__

#include <avr/io.h>

#define SET_PORT_PIN(port, pin) (port |= (1 << pin))
#define CLEAR_PORT_PIN(port, pin) (port &= ~(1 << pin))
#define READ_PORT_PIN(port, pin) ((port >> pin) & 0x01)
#define SET_PORT(port, value) (port = value)

enum pin_value {LOW = 0, HIGH};

typedef struct port_pin {
   volatile uint8_t* port;
   volatile uint8_t pin;
} port_pin_t;


#endif
