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
#include <stdio.h>

static uint8_t extraFlags = 0;

#define SEND(ack, flags) { uart_send("--send"); TWCR = (flags) | extraFlags | ((ack) << TWEA) | (1 << TWEN) | (1 << TWINT); }
#define STOP() SEND(0, 0)

#define WAIT() { uart_send("--wait"); while(!(TWCR & (1 << TWINT))); }
#define SEND_AND_WAIT(ack, flags) { SEND(ack, flags); WAIT(); }
#define DUMP_ERROR() { char str[20]; sprintf(str, "Err = %x (%x)", TW_STATUS, 0xA); uart_send(str); }

static uint8_t (*get_callback)(uint8_t) = 0;

static void (*put_callback)(uint8_t, uint8_t) = 0;

void twi_init(uint8_t address)
{
    // Set the address register
    TWAR = (address << 1) & (~0x1);

    // Divide to ~100kHz
    TWBR = 92;

    // Enable the TWI interface
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
}

void twi_enable_interrupt(void)
{
    extraFlags = 1 << TWIE;
    SEND(1, 0);
}

bool send_address(uint8_t address, uint8_t write)
{
    int c = 0;
begin:
    uart_send("START");
    // Set the START flag

    SEND_AND_WAIT(1, 1 << TWSTA);

    if (TW_STATUS != TW_START && TW_STATUS != TW_REP_START) {
        DUMP_ERROR();
        return false;
    }

    if (write == TW_WRITE) {
        uart_send("SLA+W");
    } else {
        uart_send("SLA+R");
    }

    // Write the address + write byte
    TWDR = (address << 1) | write;
    SEND_AND_WAIT(1, 0);

    uint8_t ack;
    uint8_t nack;
    if (write == TW_WRITE) {
        ack = TW_MT_SLA_ACK;
        nack = TW_MT_SLA_NACK;
    } else {
        ack = TW_MR_SLA_ACK;
        nack = TW_MR_SLA_NACK;
    }

    if (TW_STATUS == nack) {
        c++;
        if (c > 3) {
            DUMP_ERROR();
            return false;
        } else {
            goto begin;
        }
    } else if (TW_STATUS != ack) {
        DUMP_ERROR();
        return false;
    }

    return true;
}

bool twi_put(uint8_t address, uint8_t type, uint8_t data)
{
    send_address(address, TW_WRITE);

    // Send a PUT packet
    TWDR = 0;
    SEND_AND_WAIT(1, 0);

    if (TW_STATUS != TW_MT_DATA_ACK) {
        return false;
    }

    // Send the TYPE byte
    TWDR = type;
    SEND_AND_WAIT(1, 0);

    if (TW_STATUS != TW_MT_DATA_ACK) {
        return false;
    }

    // Send the data packet
    TWDR = data;
    SEND_AND_WAIT(1, 0);

    if (TW_STATUS != TW_MT_DATA_ACK) {
        return false;
    }

    STOP();

    return true;
}

bool twi_get(uint8_t address, uint8_t type, uint8_t* data)
{
    if (!send_address(address, TW_WRITE)) {
        uart_send("Send address failed");
        return false;
    }

    uart_send("Send GET");

    // Send a GET packet
    TWDR = 1;
    SEND_AND_WAIT(1, 0);

    if (TW_STATUS != TW_MT_DATA_ACK) {
        DUMP_ERROR();
        return false;
    }

    TWDR = type;
    SEND_AND_WAIT(1, 0);

    if (TW_STATUS != TW_MT_DATA_ACK) {
        DUMP_ERROR();
        return false;
    }

    uart_send("Switch to MR");
    if (!send_address(address, TW_READ)) {
        uart_send("MR send_address failed");
        STOP();
        return false;
    }

    SEND_AND_WAIT(0, 0);

    if (TW_STATUS != TW_MR_DATA_ACK && TW_STATUS != TW_MR_DATA_NACK) {
        DUMP_ERROR();
        return false;
    }

    uart_send("Read packet");
    // Read the data packet
    *data = TWDR;

    char val[20];
    sprintf(val, "--read = %x", *data);
    uart_send(val);

    STOP();

    return true;
}

void twi_register_get(uint8_t (*get)(uint8_t type))
{
    get_callback = get;
}

void twi_register_put(void (*put)(uint8_t type, uint8_t data))
{
    put_callback = put;
}

static uint8_t command = 0xFF;

static uint8_t type = 0xFF;

static char str[20];

ISR(TWI_vect, ISR_BLOCK) {

//    status_set(true);
    uart_send("TWI ISR");
    DUMP_ERROR();
    if (TW_STATUS == TW_SR_SLA_ACK) {

        uart_send("Got my SLA+W");
        command = 0xFF;
        type = 0xFF;

    } else if (TW_STATUS == TW_ST_SLA_ACK) {

        if (command == 0x0) {
            uart_send("Got into a ST with a PUT --- wwwwat");
        } else if (command == 0x1) {

            if (type == 0xFF) {

                type = TWDR;
                sprintf(str, "Got GET of type = %x", type);
                uart_send(str);

            } else {

                if (get_callback) {
                    TWDR = get_callback(type);
                } else {
                    uart_send("No callback, sending 0xFF");
                    TWDR = 0xFF;
                }
            }

            // For the last data byte we need to set ack to 0
            SEND(0, 0);
            return;
        }

    } else if (TW_STATUS == TW_SR_DATA_ACK) {

        if (command == 0xFF) {
            command = TWDR;

            sprintf(str, "Got command = %x", command);
            uart_send(str);

        } else if (type == 0xFF) {

            type = TWDR;

            sprintf(str, "Got type = %x", type);
            uart_send(str);

        } else {

            if (command == 0x00) {
                if (put_callback) {
                    put_callback(TWDR);
                }
            } else {
                uart_send("WTF - Got DATA on GET");
            }
        }

    } else if (TW_STATUS == TW_ST_DATA_ACK) {

        uart_send("WTF - Got DATA ACK on ST");

    }

    SEND(1, 0);
}
