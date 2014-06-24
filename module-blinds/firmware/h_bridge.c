#include "h_bridge.h"

#include <stdlib.h>

#include "io.h"

typedef struct bridge_t {
	port_pin_t* enable;
	port_pin_t* a;
	port_pin_t* b;
} bridge_t;

static bridge_t* bridges[10];

static uint8_t bridges_count = 0;

uint8_t register_bridge(port_pin_t* pp_enable, port_pin_t* pp_a, port_pin_t* pp_b) {
	bridge_t* bridge = malloc(sizeof(bridge_t));
	bridge->enable = pp_enable;
	bridge->a = pp_a;
	bridge->b = pp_b;
	uint8_t id = bridges_count;
	bridges[bridges_count++] = bridge;
	return id;
}

void enable(uint8_t bridge) {
	SET_PORT_PIN(*(bridges[bridge]->enable->port), bridges[bridge]->enable->pin);
}

void disable(uint8_t bridge) {
	CLEAR_PORT_PIN(*(bridges[bridge]->enable->port), bridges[bridge]->enable->pin);
}

void set_direction(uint8_t bridge, direction_t d) {
	if (d == DIRECTION_1) {
		CLEAR_PORT_PIN(*(bridges[bridge]->b->port), bridges[bridge]->b->pin);
		SET_PORT_PIN(*(bridges[bridge]->a->port), bridges[bridge]->a->pin);
	} else if (d == DIRECTION_2) {
		CLEAR_PORT_PIN(*(bridges[bridge]->a->port), bridges[bridge]->a->pin);
		SET_PORT_PIN(*(bridges[bridge]->b->port), bridges[bridge]->b->pin);
	} else {
		CLEAR_PORT_PIN(*(bridges[bridge]->a->port), bridges[bridge]->a->pin);
		CLEAR_PORT_PIN(*(bridges[bridge]->b->port), bridges[bridge]->b->pin);
	}
}

void toggle_direction(uint8_t bridge) {
	if (READ_PORT_PIN(*(bridges[bridge]->a->port), bridges[bridge]->a->pin)) {
		set_direction(bridge, DIRECTION_2);
	} else if (READ_PORT_PIN(*(bridges[bridge]->b->port), bridges[bridge]->b->pin)) {
		set_direction(bridge, DIRECTION_1);
	}
}
