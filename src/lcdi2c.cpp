/**
 * @file lcdi2c.cpp
 *
 * Made with <3 by fluffy
 */

#include "lcdi2c.hpp"

LCDI2C::LCDI2C(int address, int width, int height, bool backlight)
: _address(address), _width(width), _height(height), _backlight(backlight ? BACKLIGHT : 0x00), _cursor_x(0), _cursor_y(0)
{}
	
void LCDI2C::init(bool multiLine, bool detailedFont, bool displayState, bool cursorState, bool cursorBlink) {
	_displayState = displayState;
	_cursorState = cursorState;
	_cursorBlink = cursorBlink;
	_detailedFont =  detailedFont;
	
	i2c_init();
	soft_reset();
	
	send_command(LCD_SET_FUNCTION
		| LCD_FUNCTION_4BIT
		| (multiLine ? LCD_FUNCTION_2LINE : LCD_FUNCTION_1LINE)
		| (detailedFont ? LCD_FUNCTION_5X10 : LCD_FUNCTION_5X8)
	);
	send_command(LCD_SET_DISPLAY
		| (displayState ? LCD_DISPLAY_ON : LCD_DISPLAY_OFF)
		| (cursorState ? LCD_CURSOR_ON : LCD_CURSOR_OFF)
		| (cursorBlink ? LCD_BLINKING_ON : LCD_BLINKING_OFF)
	);
	send_command(LCD_SET_ENTRY | LCD_ENTRY_INCREASE | LCD_ENTRY_NOSHIFT);
	home();
	clear();
}

void LCDI2C::setBacklight(bool state) {
	_backlight = state ? BACKLIGHT : 0x00;
	i2c_write(_address, &_backlight, 1);
}

void LCDI2C::setDisplayState(bool state) {
	_displayState = state;
	update_display_mode();
}

void LCDI2C::setCursorState(bool state) {
	_cursorState = state;
	update_display_mode();
}

void LCDI2C::setCursorBlink(bool state) {
	_cursorBlink = state;
	update_display_mode();
}

void LCDI2C::clear() {
	send_command(LCD_CLEAR_DISPLAY);
	_delay_ms(LCD_CLEAR_DELAY);
}

void LCDI2C::write(const char* text) {
	while (*text != '\0') {
		if (_cursor_x >= _width) {
			if (_cursor_y >= (_height - 1)) home();
			else move(0, _cursor_y + 1);
		}
		send_data(*text++);
		_cursor_x ++;
	}
}

void LCDI2C::write(const char* text, int length) {
	for (int i = 0; i < length; i++) {
		if (_cursor_x >= _width) {
			if (_cursor_y >= (_height - 1)) home();
			else move(0, _cursor_y + 1);
		}
		send_data(*text++);
		_cursor_x ++;
	}
}

void LCDI2C::home() {
	send_command(LCD_CURSOR_HOME);
	_delay_ms(LCD_CURSOR_HOME_MS);
	_cursor_x = 0;
	_cursor_y = 0;
}

void LCDI2C::move(uint8_t x, uint8_t y) {
	_cursor_x = x;
	_cursor_y = y;
	send_command(LCD_SET_DDADR + get_current_address_position());
}

void LCDI2C::send_nibble(uint8_t nibble, bool rs) {
	uint8_t data[2];
	
	data[0] = nibble | EN_BIT | (rs ? RS_BIT : 0x00) | _backlight;
	data[1] = nibble | _backlight;
	
	i2c_write(_address, data, 1);
	_delay_us(LCD_ENABLE_US);
	i2c_write(_address, &data[1], 1);
}

void LCDI2C::send_command(uint8_t cmd, bool rs) {
	uint8_t high_nibble = cmd & 0xF0;
	uint8_t low_nibble = (cmd << 4) & 0xF0;
	
	send_nibble(high_nibble, rs);
	send_nibble(low_nibble, rs);
	_delay_us(LCD_COMMAND_US);
}

void LCDI2C::soft_reset() {
	_delay_ms(LCD_BOOTUP_MS);
	send_nibble(LCD_SOFT_RESET);
    _delay_ms(LCD_SOFT_RESET_MS_1);
    send_nibble(LCD_SOFT_RESET);
    _delay_ms(LCD_SOFT_RESET_MS_2);
    send_nibble(LCD_SOFT_RESET);
    _delay_ms(LCD_SOFT_RESET_MS_3);
    send_nibble(LCD_SET_FUNCTION | LCD_FUNCTION_4BIT);
    _delay_ms(LCD_4BITMODE_MS);
}

void LCDI2C::update_display_mode() {
	send_command(LCD_SET_DISPLAY
	| (_displayState ? LCD_DISPLAY_ON : LCD_DISPLAY_OFF)
	| (_cursorState ? LCD_CURSOR_ON : LCD_CURSOR_OFF)
	| (_cursorBlink ? LCD_BLINKING_ON : LCD_BLINKING_OFF)
	);
}

void LCDI2C::send_data(uint8_t byte) {
	send_command(byte, true);
}

void LCDI2C::generateChar(uint8_t address, const uint8_t *data) {
	send_command(LCD_SET_CGADR | (address << 3));
	for (uint8_t i = 0; i < (_detailedFont ? 10 : 8); i++) send_data(data[i]);
	send_command(LCD_SET_DDADR);
}

int LCDI2C::get_current_address_position() {
	if (_cursor_y == 0) return _cursor_x;
	if (_cursor_y == 1) return 0x40 + _cursor_x;
	if (_cursor_y == 2) return _width + _cursor_x;
	if (_cursor_y == 3) return 0x40 + _width + _cursor_x;
	return 0;
}