//
//  common.h
//  Test
//
//  Created by Juan Pablo Civile on 22/04/14.
//
//

#ifndef common_common_h
#define common_common_h


#include <stdint.h>

typedef uint8_t bool;

#define true 1
#define false 0

#define BIT_MASK(b7, b6, b5, b4, b3, b2, b1, b0) ( \
((b7) << 7) | ((b6) << 6) | ((b5) << 5) | ((b4) << 4) | ((b3) << 3) | ((b2) << 2) | ((b1) << 1) | ((b0)) \
)

#endif
