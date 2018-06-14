/***************
 *
 * Hello World - OLED print sample
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
  oled.begin();
  // oled.mirror();
  // oled.flip();
  oled.println("Hello World");
}

void loop (void) {
  oled.println(millis(), DEC);
  oled.moveCursor(0, -1);
}

// end of code
