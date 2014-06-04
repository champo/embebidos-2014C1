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
bool twi_put(uint8_t address, uint8_t data);

bool twi_get(uint8_t address, uint8_t* data);

void twi_register_get(uint8_t (*get)(void));

void twi_register_put(void (*put)(uint8_t data));

#endif
