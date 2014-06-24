//
//  switch.h
//  Test
//
//  Created by Juan Pablo Civile on 22/04/14.
//
//

#ifndef Test_switch_h
#define Test_switch_h

#include "common.h"

#define IS_SWITCH_ON(switch, value) ((~(value) >> (switch)) & 0x01)

void switch_init(void);

uint8_t switch_read(void);


#endif
