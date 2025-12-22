/**
 * @file lcdi2c.hpp
 *
 * Made with <3 by fluffy
 */

#ifndef LCD_H_
#define LCD_H_

#include "i2c.hpp"
#include <util/delay.h>

// timings
#define LCD_BOOTUP_MS 40
#define LCD_SOFT_RESET_MS_1 5
#define LCD_SOFT_RESET_MS_2 1
#define LCD_SOFT_RESET_MS_3 1
#define LCD_4BITMODE_MS 5
#define LCD_ENABLE_US 50
#define LCD_COMMAND_US 42
#define LCD_CLEAR_DELAY 2
#define LCD_WRITEDATA_US 46
#define LCD_CURSOR_HOME_MS 2

// bits
#define BACKLIGHT 0x08
#define EN_BIT 0x04
#define RS_BIT 0x01

// commands
#define LCD_CLEAR_DISPLAY       0x01

// Cursor Home ---------------- 0b0000001x
#define LCD_CURSOR_HOME         0x02

// Set Entry Mode ------------- 0b000001xx
#define LCD_SET_ENTRY           0x04

#define LCD_ENTRY_DECREASE      0x00
#define LCD_ENTRY_INCREASE      0x02
#define LCD_ENTRY_NOSHIFT       0x00
#define LCD_ENTRY_SHIFT         0x01

// Set Display ---------------- 0b00001xxx
#define LCD_SET_DISPLAY         0x08

#define LCD_DISPLAY_OFF         0x00
#define LCD_DISPLAY_ON          0x04
#define LCD_CURSOR_OFF          0x00
#define LCD_CURSOR_ON           0x02
#define LCD_BLINKING_OFF        0x00
#define LCD_BLINKING_ON         0x01

// Set Shift ------------------ 0b0001xxxx
#define LCD_SET_SHIFT           0x10

#define LCD_CURSOR_MOVE         0x00
#define LCD_DISPLAY_SHIFT       0x08
#define LCD_SHIFT_LEFT          0x00
#define LCD_SHIFT_RIGHT         0x04

// Set Function --------------- 0b001xxxxx
#define LCD_SET_FUNCTION        0x20

#define LCD_FUNCTION_4BIT       0x00
#define LCD_FUNCTION_8BIT       0x10
#define LCD_FUNCTION_1LINE      0x00
#define LCD_FUNCTION_2LINE      0x08
#define LCD_FUNCTION_5X8        0x00
#define LCD_FUNCTION_5X10       0x04

#define LCD_SOFT_RESET          0x30

// Set CG RAM Address --------- 0b01xxxxxx  (Character Generator RAM)
#define LCD_SET_CGADR           0x40

#define LCD_GC_CHAR0            0
#define LCD_GC_CHAR1            1
#define LCD_GC_CHAR2            2
#define LCD_GC_CHAR3            3
#define LCD_GC_CHAR4            4
#define LCD_GC_CHAR5            5
#define LCD_GC_CHAR6            6
#define LCD_GC_CHAR7            7

// Set DD RAM Address --------- 0b1xxxxxxx  (Display Data RAM)
#define LCD_SET_DDADR           0x80

class LCDI2C {
public:
	LCDI2C(int address, int width, int height, bool backlight);
	
	// Initialize the LCD
	void init(bool multiLine, bool detailedFont, bool displayState, bool cursorState, bool cursorBlink);
	
	// Enable/Disable Background of the LCD
	void setBacklight(bool state);
	
	// Enable/Disable Display content visibility
	void setDisplayState(bool state);
	
	// Enable/Disable Cursor visibility
	void setCursorState(bool state);
	
	// Enable/Disable Cursor blinking
	void setCursorBlink(bool state);
	
	// Clear the LCD Screen
	void clear();
	
	// Write text to Screen
	void write(const char* text);
	
	// Write text to Screen with length
	void write(const char* text, int length);
	
	// Bring cursor home
	void home();
	
	// Move cursor to x y
	void move(uint8_t x, uint8_t y);
	
	// Creates a custom char
	void generateChar(uint8_t address, const uint8_t *data);
private:
	void send_nibble(uint8_t nibble, bool rs = false);
	void send_command(uint8_t command, bool rs = false);
	void soft_reset();
	void update_display_mode();
	void send_data(uint8_t byte);
	int get_current_address_position();

	int _address;
	int _width;
	int _height;
	bool _displayState;
	bool _cursorState;
	bool _cursorBlink;
	bool _detailedFont;
	uint8_t _backlight;
	
	int _cursor_x;
	int _cursor_y;
};

#endif /* LCD_H_ */