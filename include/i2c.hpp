/**
 * @file i2c.hpp
 *
 * Made with <3 by fluffy
 */

#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// USI
#ifdef USISR

	#define I2C_MODE_USI

	#define USI_PORT_MASK ((1 << PORT_USI_SDA) | (1 << PORT_USI_SCL))
	#define I2C_DELAY_US 1 // Clock delay (us) for ~1MHz at 8MHz
	
	// Pin definitions for ATtiny85
	#define I2C_USI_DDR				DDRB
	#define I2C_USI_PORT			PORTB
	#define I2C_USI_PIN				PINB
	#define I2C_USI_PORT_SDA		PORTB0     // Pin 5
	#define I2C_USI_PORT_SCL	PORTB2     // Pin 7

#endif

// TWI
#ifdef TWSR

	#define I2C_MODE_TWI

	#define TWI_START           0x08
	#define TWI_REP_START       0x10
	#define TWI_MT_SLA_ACK      0x18
	#define TWI_MT_DATA_ACK     0x28
	#define TWI_MR_SLA_ACK      0x40
	#define TWI_MR_DATA_ACK     0x50
	#define TWI_MR_DATA_NACK    0x58
	#define SCL_CLOCK 1000000L

#endif

void i2c_init(void);

bool i2c_start(void);
void i2c_stop(void);

bool i2c_write_byte(uint8_t data);
uint8_t i2c_read_byte(uint8_t ack);

bool i2c_write(uint8_t addr, uint8_t *data, uint8_t len);
bool i2c_read(uint8_t addr, uint8_t *data, uint8_t len);