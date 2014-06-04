/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include "common.h"
#include "status.h"

int main(void)
{
    status_init();
    status_set(true);
    
    while (1);
}
