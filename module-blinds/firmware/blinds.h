#ifndef __BLINDS_H__

#define __BLINDS_H__

#include <stdint.h>

void blinds_init();

uint8_t blinds_do_get();

void blinds_do_put(uint8_t param);

void blinds_update_status();

#endif
