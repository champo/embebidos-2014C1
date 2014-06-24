#include <avr/io.h>

#define SET_PORT_PIN(port, pin) port |= (1 << 7);
#define CLEAR_PORT_PIN(port, pin) port &= ~(1 << 7);

