//
//  twi.h
//  module-temperature
//
//  Created by Juan Pablo Civile on 03/06/14.
//
//

#ifndef common_twi_h
#define common_twi_h

#include "common.h"

void twi_init(uint8_t address);

// These two will be only to play around with two 644, since the real master won't use this code
bool twi_put(uint8_t address, uint8_t type, uint8_t data);

bool twi_get(uint8_t address, uint8_t type, uint8_t* data);

void twi_register_get(uint8_t (*get)(uint8_t type));

void twi_register_put(void (*put)(uint8_t type, uint8_t data));

void twi_enable_interrupt(void);

static uint8_t LUMINOSITY = 1;
static uint8_t BLINDS_HEIGHT = 2;
static uint8_t AIR_ON_OFF = 3;
static uint8_t TEMPERATURE = 4;
static uint8_t LIGHTS = 5;
static uint8_t BLINDS_SPEED = 6;
static uint8_t AIR_TEMPERATURE = 7;

#endif
