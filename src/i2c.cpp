/**
 * @file i2c.cpp
 *
 * Made with <3 by fluffy
 */

#include "i2c.hpp"

// TWI bit rate (for 100kHz at 8MHz CPU)

// Initialize TWI (hardware) as master
void twi_master_init(void) {
	// Set SCL frequency
	TWSR = 0x00; // Prescaler = 1
	TWBR = (uint8_t)(((F_CPU / SCL_CLOCK) - 16) / 2);
	// Enable TWI
	TWCR = (1 << TWEN);
}

// Send START condition
// Returns 0 if bus busy or error
uint8_t twi_start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	// Wait for TWINT Flag set
	while (!(TWCR & (1 << TWINT)));

	uint8_t status = TWSR & 0xF8;
	if ((status != TW_START) && (status != TW_REP_START)) {
		return 0; // Failed to start
	}
	return 1; // Success
}

// Send STOP condition
void twi_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	// TWSTO is cleared automatically
}

// Write a byte to the bus
// Returns 0 on success (ACK received), 1 on NACK
uint8_t twi_write_byte(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));

	uint8_t status = TWSR & 0xF8;
	if ((status != TW_MT_SLA_ACK) && (status != TW_MT_DATA_ACK) &&
	(status != TW_MR_DATA_ACK)) {
		return 1; // NACK or error
	}
	return 0;
}

// Read a byte from the bus
// ack = 0: send NACK after byte (last byte), ack = 1: send ACK
uint8_t twi_read_byte(uint8_t ack) {
	if (ack) {
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
		} else {
		TWCR = (1 << TWINT) | (1 << TWEN); // NACK
	}
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

// Write multiple bytes to a slave
// addr: 7-bit slave address
// data: pointer to buffer
// len: number of bytes
// Returns 0 on success, 1 on failure
uint8_t twi_write(uint8_t addr, uint8_t *data, uint8_t len) {
	if (!twi_start()) return 1;
	if (twi_write_byte((addr << 1) | 0)) { // write bit
		twi_stop();
		return 1;
	}
	for (uint8_t i = 0; i < len; i++) {
		if (twi_write_byte(data[i])) {
			twi_stop();
			return 1;
		}
	}
	twi_stop();
	return 0;
}

// Read multiple bytes from a slave
// addr: 7-bit slave address
// data: pointer to buffer
// len: number of bytes
// Returns 0 on success, 1 on failure
uint8_t twi_read(uint8_t addr, uint8_t *data, uint8_t len) {
	if (!twi_start()) return 1;
	if (twi_write_byte((addr << 1) | 1)) { // read bit
		twi_stop();
		return 1;
	}
	for (uint8_t i = 0; i < len; i++) {
		data[i] = twi_read_byte(i < (len - 1)); // ACK all but last
	}
	twi_stop();
	return 0;
}