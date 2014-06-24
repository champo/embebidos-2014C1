#define F_CPU 1000000UL  // 1 MHz

#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>


#include "h_bridge.h"

#define RECEIVERS 4
#define RECEIVER0_PORT PINA
#define RECEIVER0_PIN 0
#define RECEIVER1_PORT PINA
#define RECEIVER1_PIN 1
#define RECEIVER2_PORT PINA
#define RECEIVER2_PIN 2
#define RECEIVER3_PORT PINA
#define RECEIVER3_PIN 3

#define CMD_WAIT 500

#define DOWN DIRECTION_1
#define UP DIRECTION_2

typedef enum {FOLDED, UNFOLDED, DOING_PUT, WAITING} status_t;

typedef enum {OFF_OFF, OFF_ON, ON_ON, ON_OFF} counter_status_t;

uint8_t receiver_status[RECEIVERS];
port_pin_t* receiver_ports[RECEIVERS];

uint8_t bridge_id;

void (*receiver_toggles[RECEIVERS])(uint8_t);

status_t blinds_status;

uint8_t target_height;
uint8_t current_height;
uint8_t bottom_height;

direction_t direction;

static counter_status_t counter_status;

uint8_t blinds_read_receiver(uint8_t receiver) {
	port_pin_t* pp = receiver_ports[receiver];
	return READ_PORT_PIN(*(pp->port), pp->pin);
}

void blinds_turn_switch_off() {
	blinds_status = WAITING;
	set_direction(bridge_id, OFF);
}

void blinds_receiver0_do_toggle(uint8_t new_value) {
	if (new_value) {
		// Top receiver reads something, meaning that the blind
		// is completely folded.
		blinds_turn_switch_off();
		blinds_status = FOLDED;
	}
}

void blinds_receiver1_do_toggle(uint8_t new_value) {
	if (new_value) {
		if (counter_status == OFF_ON) {
			counter_status = ON_ON;
		} else if (counter_status == OFF_OFF) {
			counter_status = ON_OFF;
			current_height++;
			direction = DOWN;
			if (current_height == target_height) {
				blinds_turn_switch_off();
			}
		}
	} else {
		if (counter_status == ON_ON) {
			counter_status = OFF_ON;
		} else if (counter_status == ON_OFF) {
			counter_status = OFF_OFF;
		}
	}
}

void blinds_receiver2_do_toggle(uint8_t new_value) {
	if (new_value) {
		if (counter_status == OFF_OFF) {
			counter_status = OFF_ON;
			current_height--;
			if (current_height < 0) {
				current_height = 0;
			}
			direction = UP;
			if (current_height == target_height) {
				blinds_turn_switch_off();
			}
		} else if (counter_status == ON_OFF) {
			counter_status = ON_ON;
		}
	} else {
		if (counter_status == ON_ON) {
			counter_status = ON_OFF;
		} else if (counter_status == OFF_ON) {
			counter_status = OFF_OFF;
		}
	}
}

void blinds_receiver3_do_toggle(uint8_t new_value) {
	if (!new_value) {  
		// Bottom receiver stopped reading, meaning that the blind
		// should stop going downwards.
		blinds_turn_switch_off();
		blinds_status = UNFOLDED;
		if (target_height >0) {
			target_height = 0;
		}
		bottom_height = current_height;
	}
}

void receiver_init(uint8_t receiver, volatile uint8_t* port, uint8_t pin) {
	port_pin_t* pp = malloc(sizeof(port_pin_t));
	pp->port = port;
	pp->pin = pin;
	receiver_ports[receiver] = pp;
}

void blinds_init() {
	SET_PORT_PIN(DDRD, 5);
	SET_PORT_PIN(DDRD, 6);

	port_pin_t a = {.port = &PORTD, .pin = 5};
	port_pin_t b = {.port = &PORTD, .pin = 6};
	bridge_id = register_bridge(NULL, &a, &b);
	int i;
	receiver_init(0, &RECEIVER0_PORT, RECEIVER0_PIN);
	receiver_toggles[0] = blinds_receiver0_do_toggle;
	receiver_init(1, &RECEIVER1_PORT, RECEIVER1_PIN);
	receiver_toggles[1] = blinds_receiver1_do_toggle;
	receiver_init(2, &RECEIVER2_PORT, RECEIVER2_PIN);
	receiver_toggles[2] = blinds_receiver2_do_toggle;
	receiver_init(3, &RECEIVER3_PORT, RECEIVER3_PIN);
	receiver_toggles[3] = blinds_receiver3_do_toggle;
	for (i = 0 ; i < RECEIVERS ; i++) {		
		receiver_status[i] = blinds_read_receiver(i);
	}
	if (receiver_status[1] && receiver_status[2]) {
		counter_status = ON_ON;
	} else if (!receiver_status[1] && receiver_status[2]) {
		counter_status = OFF_ON;
	} else if (receiver_status[1] && !receiver_status[2]) {
		counter_status = ON_OFF;
	} else {
		counter_status = OFF_OFF;
	}
	target_height = -1;
	// Assume blinds start at height 0
	current_height = 0;
	blinds_status = FOLDED;
	direction = OFF;
	bottom_height = 0;
}

void blinds_do_toggle(uint8_t receiver, uint8_t new_value) {
	receiver_toggles[receiver](new_value);
}

void blinds_turn_switch_on_up() {
	blinds_status = DOING_PUT;
	set_direction(bridge_id, UP);
}

void blinds_turn_switch_on_down() {
	blinds_status = DOING_PUT;
	set_direction(bridge_id, DOWN);
}

void blinds_update_status() {
	int status, i;
	for (i = 0 ; i < RECEIVERS ; i++) {
		status = blinds_read_receiver(i);
		if (status != receiver_status[i]) {
			blinds_do_toggle(i, status);
			receiver_status[i] = status;
		}
	}
}

void blinds_do_put(uint8_t height) {
	// Check if should put, queue, ignore or update.
	if (blinds_status == DOING_PUT &&
		(!(direction == UP && height < current_height) ||
		!(direction == DOWN && height > current_height))) {
		blinds_turn_switch_off();
		_delay_ms(500);
	}
	target_height = height;
	if (blinds_status != DOING_PUT) {
		blinds_status = DOING_PUT;
		if (current_height < target_height) {
			blinds_turn_switch_on_down();
		} else {
			blinds_turn_switch_on_up();
		}
	}
	
}

uint8_t blinds_do_get() {
	if (bottom_height == 0) {
		return 0xFF;
	}
	return (current_height * 100) / bottom_height;
}
