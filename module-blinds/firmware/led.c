#include "led.h"
#include "io.h"

static void led_set(uint8_t value) {
	SET_PORT_PIN(LED_DDR, LED_PIN);
	if (value) {
		SET_PORT_PIN(LED_PORT_WRITE, LED_PIN);
	} else {
		CLEAR_PORT_PIN(LED_PORT_WRITE, LED_PIN);
	}
}

void led_on() {
	led_set(LED_ON);
}

void led_off() {
	led_set(LED_OFF);
}

void led_toggle() {
	led_set(~READ_PORT_PIN(LED_PORT_READ, LED_PIN));
}
