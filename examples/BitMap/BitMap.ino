/***************
 *
 * BitMap - OLED bitmap sample
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/GPS_MTK333X
 * maintainer: askn https://twitter.com/askn37
 *
 */

#include <Arduino.h>
#include "OLED_SSD1306.h"

OLED_M091P oled;

void setup (void) {
	oled.begin();
}

volatile uint8_t bits[16];
volatile uint8_t count = 0;
void loop (void) {
	for (int k = 0; k < 4; k++) {
		oled.setCursor(0, k);
		for (int j = 0; j < 16; j++) {
			for (int i = 0; i < 16; i++) bits[i] = count++;
			oled.rawWrite(bits);
		}
	}
	oled.vOffset(0x3F & count++);
}

// end of code
