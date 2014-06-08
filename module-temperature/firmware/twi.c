//
//  twi.c
//  module-temperature
//
//  Created by Juan Pablo Civile on 03/06/14.
//
//

#include "twi.h"

#include "uart.h"

#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#define SEND { uart_send("--send"); TWCR |= (1 << TWINT); }
#define WAIT { uart_send("--wait"); while(!(TWCR & (1 << TWINT))); }
#define SEND_AND_WAIT { SEND; WAIT; }

static uint8_t (*get_callback)(void) = 0;

static void (*put_callback)(uint8_t) = 0;

void twi_init(uint8_t address)
{
    // Set the address register
    TWAR = address << 1;
    
    // Don't set a bit register
    TWBR = 0;
    
    // Enable the TWI interface
    TWCR = (1 << TWEA) | (1 << TWEN);
 
    // Prescale by 16
    TWSR = (1 << TWPS1) | (1 << TWPS0);
}

bool send_address(uint8_t address, uint8_t write)
{
    uart_send("START");
    // Set the START flag
    TWCR |= 1 << TWSTA;
    
    SEND_AND_WAIT;
    
    if (TW_STATUS != TW_START && TW_STATUS != TW_REP_START) {
        return false;
    }
    
    uart_send("SLA+W");
    // Write the address + write byte
    TWDR = (address << 1) | write;
    TWCR |= (1 << TWINT);
    
    SEND_AND_WAIT;
    
    uint8_t ack;
    if (write == TW_WRITE) {
        ack = TW_MT_SLA_ACK;
    } else {
        ack = TW_MR_SLA_ACK;
    }
    
    if (TW_STATUS != ack) {
        return false;
    }
    
    return true;
}

bool twi_put(uint8_t address, uint8_t data)
{
    send_address(address, TW_WRITE);
    
    // Send a PUT packet
    TWDR = 0;
    SEND_AND_WAIT;
    
    if (TW_STATUS != TW_MT_DATA_ACK) {
        return false;
    }
    
    send_address(address, TW_WRITE);
    
    // Send the data packet
    TWDR = data;
    SEND_AND_WAIT;
    
    if (TW_STATUS != TW_MT_DATA_ACK) {
        return false;
    }
    
    TWCR |= 1 << TWSTO;
    
    SEND_AND_WAIT;
    
    return true;
}

bool twi_get(uint8_t address, uint8_t* data)
{
    send_address(address, TW_WRITE);
    
    uart_send("Send GET");
    
    // Send a GET packet
    TWDR = 1;
    SEND_AND_WAIT;
    
    if (TW_STATUS != TW_MT_DATA_ACK) {
        return false;
    }
    
    uart_send("Switch to MR");
    send_address(address, TW_READ);
    
    WAIT;
    
    if (TW_STATUS != TW_MR_DATA_ACK) {
        return false;
    }
    
    uart_send("Read packet");
    // Read the data packet
    *data = TWDR;
    
    TWCR |= 1 << TWSTO;
    
    SEND_AND_WAIT;
    
    return true;
}

void twi_register_get(uint8_t (*get)(void))
{
    get_callback = get;
}

void twi_register_put(void (*put)(uint8_t data))
{
    put_callback = put;
}

ISR(TWI_vect, ISR_BLOCK) {
    
    if (TW_STATUS == TW_SR_SLA_ACK) {
        
        if (put_callback) {
            put_callback(TWDR);
        }
        
    } else if (TW_STATUS == TW_ST_DATA_ACK) {
        
        if (get_callback) {
            TWDR = get_callback();
        } else {
            TWDR = 0xFF;
        }
        
    } else {
        // Keep the enable alive, just in case
        TWCR |= 1 << TWEA;
    }
    
    SEND;
}
