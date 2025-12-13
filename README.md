# LCDDriverI2C
A driver for any LCD Module using I²C-Communication.

## Installation
For installation put `lcdi2c.cpp` and `lcdi2c.hpp` in your project. You are also required to put the `i2c.hpp` and `i2c.cpp` in your project. All header files are in the `/include` folder and all source files are in the `/src` folder. In an AVR Project they go all in the same root folder.

## Usage
**NOTE:** If you forget something, in the header files are also small documentation hints

### First Steps
First you need to create an LCD Object. Example here:
```cpp
LCDI2C lcd = LCDI2C(0x27, 16, 2, true);
```
In that example we created an LCD Display at Address `0x27` (mostly changable with soldering contacts on the I²C Driver) with 16 characters per Line. 2 Lines in total and the `true` at the end toggles the Backlight at init. The backlight can also be changed later.

### Initialization
For initialization you need to run the following:
```cpp
lcd.init(true, false, true, false, false);
```
This Example initialized the lcd with the following values:
| Position | Title | State | Info |
| - | - | - | - |
| 0 | multiLine | true | Checks if the LCD display uses multiple lines or not |
| 1 | detailedFont | false | Detailed font means 5x10 Characters. Default is 5x8
| 2 | displayState | true | If enabled, the display content is visible
| 3 | cursorState | false | If enabled, the cursor is visible
| 4 | cursorBlink | false | If enabled AND cursorState is enabled, the cursor will blink

You can modify each value how you like.

### Writing
Writing to the display is as easy as the setup. Just do the following:
```cpp
lcd.write("Hello, World!");
```
This will write `Hello, World!` to the Display. You can write anything you want. If you write more than your character length, the driver will AUTOMATICALLY increment to the next line. If the end of the screen is reached, it will go automatically to the home position. (X=0, Y=0).

### Clear Screen
You can clear the screen with the following method:
```cpp
lcd.clear();
```

### Cursor
You can not only adjust the Cursor, you can also move it!
```cpp
// Going Home
lcd.home();

// Go to X = 1, Y = 2
lcd.move(1, 2);
```
Comments are pretty self explaining.

### Adjusting Display
You can adjust the display in a few ways:
```cpp
// Backlight
lcd.setBacklight(true);

// Display Content Visibility
lcd.setDisplayState(true);

// Cursor Visibility
lcd.setCursorState(true);

// Cursor Blinking
lcd.setCursorBlink(true);
```
As the comments say, Backlight, Display Content Visibility, Cursor Visibility and Cursor Blinking can be changed later on.

### Custom Characters
There is also a possiblity to create custom characters. The characters are mapped from the top to the bottom in a `uint8_t` array. Each bit is a pixel at screen. If you have a 5x8 character screen, the first 5 bits and 8 of them are processed. If it is a 5x10 character screen, the first 5 bits and 10 of them are processed.

You can create an example character like that or using a cool website for creation [here](https://omerk.github.io/lcdchargen/):
```cpp
const uint8_t smileyFace[] = {
    0b00000,
    0b01010,
    0b01010,
    0b00000,
    0b11111,
    0b11111,
    0b01110,
    0b00000
};
lcd.generateChar(0x01, smileyFace);
```
This will load the `smileyFace` data into the LCD itself. They will stay even on powerloss! As you can see they use an Address. You can upload up to 9 different characters (depending on your screen). The first one is always 0x01, because the 0x00 is used for the string end. So possible slots are `0x01` up to `0x09` including both.

Now you created a custom character. But how do you write them now? You can just append them into the String with the `\` escape method:
```cpp
lcd.write("\x01");
```
Here you can see we escaped the `0x01` using the `\` so its `\x01`. So you can just replace the `01` with every other possible number. You can also write text before and after the custom character.
```cpp
lcd.write("A smiley Face: \x01");
```

###### Made with <3 by fluffy
