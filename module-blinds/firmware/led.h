#ifndef __LED_H__

#define __LED_H__

#include "io.h"

#define LED_DDR DDRD
#define LED_WRITE 1
#define LED_PIN 7
#define LED_PORT_WRITE PORTD
#define LED_PORT_READ PIND
#define LED_OFF 1
#define LED_ON 0

void led_on();

void led_off();

void led_toggle();

#endif
