#ifndef __H_BRIDGE_H__

#define __H_BRIDGE_H__

#include "io.h"

#define ENABLE_PORT PORTD
#define ENABLE_PIN 4

typedef enum {DIRECTION_1, DIRECTION_2, OFF} direction_t;

uint8_t register_bridge(port_pin_t* pp_enable, port_pin_t* pp_a, port_pin_t* pp_b);

void enable(uint8_t bridge);

void disable(uint8_t bridge);

void set_direction(uint8_t bridge, direction_t d);

void toggle_direction(uint8_t bridge);

#endif 
