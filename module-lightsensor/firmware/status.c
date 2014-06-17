//
//  status.c
//  Test
//
//  Created by Juan Pablo Civile on 22/04/14.
//
//

#include "status.h"
#include <avr/io.h>
#include "common.h"

#define STATUS_MASK (1 << 7)

void status_init(void) {
    // Enable write on all leds
    DDRD |= STATUS_MASK;
    // Set all leds to off
    PORTD |= STATUS_MASK;
}

void status_set(uint8_t status) {
    PORTD = ((~status) << 7) | (PORTD & ~STATUS_MASK);
}
