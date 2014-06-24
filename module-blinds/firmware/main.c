#include <avr/interrupt.h>

#include "blinds.h"
#include "led.h"
#include "twi.h"

int
main() {
	led_on();
	blinds_init();
	
	// Register PUT and GET callbacks.

	twi_init(0x0E);
	twi_enable_interrupt();
	twi_register_get(blinds_do_get);
	twi_register_put(blinds_do_put);
	sei();
	while(1) {
		blinds_update_status();
	}
	return 0;
}
