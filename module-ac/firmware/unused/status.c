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

#define STATUS_MASK BIT_MASK(1, 1, 1, 1, 1, 1, 1, 1)

void status_init(void) {
    // Enable write on all leds
    DDRB = STATUS_MASK;
    // Set all leds to off
    PORTB = STATUS_MASK;
}

void status_set(uint8_t status) {
    PORTB = (~status) & STATUS_MASK;
}
