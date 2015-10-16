/**
 * remote.ino: Convert an Infrared remote control keypad to Bluetooth HID
 * device using a BlueFruit USB interface.
 *
 * Copyright (c) 2015 by Alec Smecher.
 * http://cassettepunk.com/blog/2015/10/15/tiny-tv/
 */

#include <Keypad.h>

// Keypad configuration: Adjust layout and pins as needed
const byte ROWS = 6;
const byte COLS = 4;
char keys[ROWS][COLS] = {
	{'1', '6', 's', 'p'},
	{'2', '7', 'f', 'F'},
	{'3', '8', 'c', 'C'},
	{'4', '9', 'm', 'M'},
	{'5', '0', 'l', 'r'},
	{'s', 'w', 'x', 'y'}
};
byte rowPins[ROWS] = {2, 3, 4, 5, 6, 7};
byte colPins[COLS] = {8, 9, 10, 11};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/**
 * Send a HID mouse command to the BlueFruit.
 * See https://learn.adafruit.com/introducing-bluefruit-ez-key-diy-bluetooth-hid-keyboard/sending-keys-via-serial
 */
void mouseCommand(uint8_t buttons, uint8_t x, uint8_t y) {
	Serial.write(0xFD);
	Serial.write((byte)0x00);
	Serial.write((byte)0x03);
	Serial.write(buttons);
	Serial.write(x);
	Serial.write(y);
	Serial.write((byte)0x00);
	Serial.write((byte)0x00);
	Serial.write((byte)0x00);
}

/**
 * Send a HID consumer key command to the BlueFruit.
 * See https://learn.adafruit.com/introducing-bluefruit-ez-key-diy-bluetooth-hid-keyboard/sending-keys-via-serial
 */
void consumerCommand(uint8_t mask0, uint8_t mask1) {
	Serial.write(0xFD);
	Serial.write((byte)0x00);
	Serial.write((byte)0x02);
	Serial.write(mask0);
	Serial.write(mask1);
	Serial.write((byte)0x00);
	Serial.write((byte)0x00);
	Serial.write((byte)0x00);
	Serial.write((byte)0x00);
}

/**
 * Send a HID raw key command to the BlueFruit.
 * See https://learn.adafruit.com/introducing-bluefruit-ez-key-diy-bluetooth-hid-keyboard/sending-keys-via-serial
 */
void keyCommand(uint8_t modifiers, uint8_t keycode1, uint8_t keycode2 = 0, uint8_t keycode3 = 0,
		uint8_t keycode4 = 0, uint8_t keycode5 = 0, uint8_t keycode6 = 0) {
	Serial.write(0xFD);
	Serial.write(modifiers);
	Serial.write((byte)0x00);
	Serial.write(keycode1);
	Serial.write(keycode2);
	Serial.write(keycode3);
	Serial.write(keycode4);
	Serial.write(keycode5);
	Serial.write(keycode6);
}

void setup(){
	Serial.begin(9600);
}

char lastKey; // Keep track of the last key that was pressed.

void loop() {
	char key = keypad.getKey();
	if (key != NO_KEY) {
		// Process a keypress
		switch (key) {
			case 'f': mouseCommand(0, -1, 0); break; // Left (one press)
			case 'F': mouseCommand(0, 1, 0); break; // Right (one press)
			case 'c': mouseCommand(0, 0, -1); break; // Up (one press)
			case 'C': mouseCommand(0, 0, 1); break;	// Down (one press)
			case 'm': mouseCommand(1, 0, 0); mouseCommand(0, 0, 0);break; // Click; release
			case 'M': mouseCommand(2, 0, 0); mouseCommand(0, 0, 0);break; // Click; release
			case 'p': consumerCommand(1, 0); consumerCommand(0, 0); break; // Home; release
			case 'l': keyCommand(0, 0x2a); keyCommand(0, 0); break; // Backspace
			case 'r': Serial.write('\n'); break;
			default: Serial.write(key); // Numbers
		}
		lastKey = key;
	}
	switch (keypad.getState()) {
		case HOLD:
			// Process a held key
			switch (lastKey) {
				case 'f': mouseCommand(0, -5, 0); break; // Left (held)
				case 'F': mouseCommand(0, 5, 0); break; // Right (held)
				case 'c': mouseCommand(0, 0, -5); break; // Up (held)
				case 'C': mouseCommand(0, 0, 5); break; // Down (held)
			}
			delay(50);
			break;
	}
}
