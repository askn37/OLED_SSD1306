/***************
 *
 * SimpleSample - OLED characters sample
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/GPS_MTK333X
 * maintainer: askn https://twitter.com/askn37
 *
 */

#include <Arduino.h>
#include "OLED_SSD1306.h"

// OLED_M096P oled;
OLED_M091P oled;

void setup (void) {
	Serial.begin(9600);
	Serial.println("Startup");

	oled.begin();
}

int style = 1;
void loop (void) {
	for (int k = 0; k < 256; k++) {
		int c = k;
		for (int j = 0; j < 4; j++) {
			oled.setCursor(0, j);
			for (int i = 0; i < 16; i++) {
				oled.write(c < 32 ? 32 : c);
				c++;
			}
		}
		if (Serial.available()) {
			while (Serial.available()) Serial.read();
			style++;
			style &= 3;
			oled.mirror(style & 2);
			oled.flip(style & 1);
		}
	}
}

// end of code
