/**
 * @file i2c.hpp
 *
 * Made with <3 by fluffy
 */

#pragma once

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

// Pin definitions for ATmega325
#define DDR_USI             DDRC
#define PORT_USI            PORTC
#define PIN_USI             PINC
#define PORT_USI_SDA        PORTC4		// Pin 27
#define PORT_USI_SCL        PORTC5		// Pin 28
#define PIN_USI_SDA         PINC4
#define PIN_USI_SCL         PINC5

// TWI Status codes
#define TWI_START           0x08
#define TWI_REP_START       0x10
#define TWI_MT_SLA_ACK      0x18
#define TWI_MT_DATA_ACK     0x28
#define TWI_MR_SLA_ACK      0x40
#define TWI_MR_DATA_ACK     0x50
#define TWI_MR_DATA_NACK    0x58

#define SCL_CLOCK 1000000L

void twi_master_init(void);

// Send START condition
// Returns 0 if bus busy or error
uint8_t twi_start(void);

// Send STOP condition
void twi_stop(void);

// Write a byte to the bus
// Returns 0 on success (ACK received), 1 on NACK
uint8_t twi_write_byte(uint8_t data);

// Read a byte from the bus
// ack = 0: send NACK after byte (last byte), ack = 1: send ACK
uint8_t twi_read_byte(uint8_t ack);

// Write multiple bytes to a slave
// addr: 7-bit slave address
// data: pointer to buffer
// len: number of bytes
// Returns 0 on success, 1 on failure
uint8_t twi_write(uint8_t addr, uint8_t *data, uint8_t len);

// Read multiple bytes from a slave
// addr: 7-bit slave address
// data: pointer to buffer
// len: number of bytes
// Returns 0 on success, 1 on failure
uint8_t twi_read(uint8_t addr, uint8_t *data, uint8_t len);