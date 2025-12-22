/**
 * @file i2c.cpp
 *
 * Made with <3 by fluffy
 */

#include "i2c.hpp"

#ifdef I2C_MODE_USI

static inline void usi_sda_high(void) { I2C_USI_PORT |= (1 << I2C_USI_PORT_SDA); }
static inline void usi_sda_low(void)  { I2C_USI_PORT &= ~(1 << I2C_USI_PORT_SDA); }

static inline void usi_scl_high(void) { I2C_USI_PORT |= (1 << I2C_USI_PORT_SCL); _delay_us(I2C_DELAY_US); }
static inline void usi_scl_low(void)  { I2C_USI_PORT &= ~(1 << I2C_USI_PORT_SCL); _delay_us(I2C_DELAY_US); }

#endif

void i2c_init(void) {
	#ifdef I2C_MODE_USI
		I2C_USI_DDR |= (1 << I2C_USI_PORT) | (1 << I2C_USI_PORT_SDA);
		usi_scl_high();
		usi_sda_high();
	#endif
}

bool i2c_start(void) {
	#ifdef I2C_MODE_USI
		usi_sda_high();
		usi_scl_high();
		_delay_us(I2C_DELAY_US);
		
		if (!(I2C_USI_PIN & (1 << I2C_USI_PORT_SDA))) return false;

		usi_sda_low();
		_delay_us(I2C_DELAY_US);
		usi_scl_low();
		return true;
	#endif
	
	#ifdef I2C_MODE_TWI
		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
		
		while (!(TWCR & (1 << TWINT)));

		uint8_t status = TWSR & 0xF8;
		if ((status != TW_START) && (status != TW_REP_START)) return false;
		return true;
	#endif
}

void i2c_stop(void) {
	#ifdef I2C_MODE_USI
		usi_sda_low();
		usi_scl_high();
		_delay_us(I2C_DELAY_US);
		usi_sda_high();
		_delay_us(I2C_DELAY_US);
	#endif
	
	#ifdef I2C_MODE_TWI
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	#endif
}

bool i2c_write_byte(uint8_t data) {
	#ifdef I2C_MODE_USI
		for (uint8_t i = 0; i < 8; i++) {
			if (data & 0x80) usi_sda_high();
			else usi_sda_low();
			data <<= 1;
			usi_scl_high();
			usi_scl_low();
		}

		I2C_USI_DDR &= ~(1 << I2C_USI_PORT_SDA);
		usi_scl_high();
		uint8_t ack = !(I2C_USI_PIN & (1 << I2C_USI_PORT_SDA));
		usi_scl_low();
		I2C_USI_DDR |= (1 << I2C_USI_PORT_SDA);
		return ack;
	#endif
	
	#ifdef I2C_MODE_TWI
		TWDR = data;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while (!(TWCR & (1 << TWINT)));

		uint8_t status = TWSR & 0xF8;
		if ((status != TW_MT_SLA_ACK) && (status != TW_MT_DATA_ACK) && (status != TW_MR_DATA_ACK))
			return true;
		return false;
	#endif
}

uint8_t i2c_read_byte(uint8_t ack) {
	#ifdef I2C_MODE_USI
		uint8_t data = 0;
		I2C_USI_DDR &= ~(1 << I2C_USI_PORT_SDA);

		for (uint8_t i = 0; i < 8; i++) {
			data <<= 1;
			usi_scl_high();
			if (I2C_USI_PIN & (1 << I2C_USI_PORT_SDA)) data |= 1;
			usi_scl_low();
		}

		I2C_USI_DDR |= (1 << I2C_USI_PORT_SDA);
		if (ack) usi_sda_low();
		else usi_sda_high();
		usi_scl_high();
		usi_scl_low();
		usi_sda_high();
		return data;
	#endif
	
	#ifdef I2C_MODE_TWI
		if (ack) {
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
			} else {
			TWCR = (1 << TWINT) | (1 << TWEN);
		}
		while (!(TWCR & (1 << TWINT)));
		return TWDR;
	#endif
}

bool i2c_write(uint8_t addr, uint8_t *data, uint8_t len) {
	if (!i2c_start()) return false;
	if (i2c_write_byte((addr << 1) | 0)) { i2c_stop(); return false; }
	for (uint8_t i = 0; i < len; i++) {
		if (i2c_write_byte(data[i])) { i2c_stop(); return false; }
	}
	i2c_stop();
	return true;
}

bool i2c_read(uint8_t addr, uint8_t *data, uint8_t len) {
	if (!i2c_start()) return false;
	if (i2c_write_byte((addr << 1) | 1)) { i2c_stop(); return false; }
	for (uint8_t i = 0; i < len; i++) data[i] = i2c_read_byte(i < (len - 1));
	i2c_stop();
	return true;
}
