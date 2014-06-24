//
//  switch.c
//  Test
//
//  Created by Juan Pablo Civile on 22/04/14.
//
//

#include "switch.h"
#include <avr/io.h>

#define SWITCH_MASK BIT_MASK(0, 0, 1, 1, 1, 1, 0, 0)

void switch_init(void) {
    // Enable read on enabled ports
    DDRC = ~SWITCH_MASK;
}

uint8_t switch_read(void) {
    return PINC & SWITCH_MASK;
}
