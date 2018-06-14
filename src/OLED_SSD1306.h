/***************
 *
 * OLED_SSD1306 - OLED SSD1306 (i2c) Interface for Arduino
 *
 * call constructor:
 *
 *		OLED_SSD1306		(i2c common 128x64)
 *		OLED_M096P			(aitendo M096P 128x64)
 *		OLED_M091P			(aitendo M091P 128x32)
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/GPS_MTK333X
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifndef __OLED_SSD1306_H_
#define __OLED_SSD1306_H_

#include <Arduino.h>
#include <Wire.h>

#define SSD1306_ADDR			0x3C

#define I2C_SPEED_STANDARD      100000
#define I2C_SPEED_DOUBLE        200000
#define I2C_SPEED_FAST          400000

// Control byte
#define SSD1306_CONTROL_CMD_SINGLE		0x80
#define SSD1306_CONTROL_CMD_STREAM		0x00
#define SSD1306_CONTROL_DATA_STREAM		0x40

// Fundamental commands (pg.28)
#define SSD1306_SET_CONTRAST			0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME	0xA4
#define SSD1306_DISPLAY_ALL_ON_IGNORE	0xA5
#define SSD1306_NORMAL_DISPLAY			0xA6
#define SSD1306_INVERT_DISPLAY			0xA7
#define SSD1306_DISPLAY_OFF				0xAE
#define SSD1306_DISPLAY_ON				0xAF

// Scrolling #defines (pg.28-30)
#define SSD1306_RIGHT_HORIZONTAL_SCROLL					0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL					0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL	0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL		0x2A
#define SSD1306_DEACTIVATE_SCROLL						0x2E
#define SSD1306_ACTIVATE_SCROLL							0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA				0xA3

// Addressing Command Table (pg.30)
#define SSD1306_SET_LOW_COLUMN			0x00
#define SSD1306_SET_HIGH_COLUMN			0x10
#define SSD1306_SET_MEMORY_ADDR_MODE	0x20    // follow with 0x00 = HORZ mode = Behave like a KS108 graphic LCD
#define SSD1306_SET_COLUMN_RANGE		0x21    // can be used only in HORZ/VERT mode - follow with 0x00 and 0x7F = COL127
#define SSD1306_SET_PAGE_RANGE			0x22    // can be used only in HORZ/VERT mode - follow with 0x00 and 0x07 = PAGE7
#define SSD1306_SET_PAGE_START_ADDRESS	0xB0

// Hardware Config (pg.31)
#define SSD1306_SET_DISPLAY_START_LINE	0x40
#define SSD1306_SET_SEGMENT_REMAP_LOW	0xA0
#define SSD1306_SET_SEGMENT_REMAP_HIGH	0xA1
#define SSD1306_SET_MULTIPLEX_RATIO		0xA8    // follow with 0x3F = 64 MUX
#define SSD1306_SET_COM_SCAN_INC		0xC0
#define SSD1306_SET_COM_SCAN_DEC		0xC8
#define SSD1306_SET_DISPLAY_OFFSET		0xD3    // follow with 0x00
#define SSD1306_SET_COM_PIN_MAP			0xDA    // follow with 0x12

// Timing and Driving Scheme (pg.32)
#define SSD1306_SET_DISPLAY_CLK_DIV		0xD5    // follow with 0x80
#define SSD1306_SET_PRECHARGE			0xD9    // follow with 0xF1
#define SSD1306_SET_VCOMH_DESELCT		0xDB    // follow with 0x30
#define SSD1306_NOP						0xE3    // NOP

// Charge Pump (pg.62)
#define SSD1306_SET_CHARGE_PUMP			0x8D    // follow with 0x14

class OLED_SSD1306 : public Print {
private :
	long _speed;
	uint8_t _addr;
	uint8_t _o;
	uint8_t _m;
	uint8_t _x, _y, _isSleep;
	bool sendOneCommand (uint8_t);
	bool sendTwoCommand (uint8_t, uint8_t);

public :
	OLED_SSD1306 (void) {
		_o = 0;
		_m = 0x12;
	}

    virtual bool begin (const long = I2C_SPEED_FAST, const uint8_t = SSD1306_ADDR);
    void setClock (long speed) { _speed = speed; Wire.setClock(_speed); }
	virtual bool clear (void);
	virtual bool newLine (const int, const int = 0);

	using Print::write;
	virtual size_t write (const uint8_t);
	virtual size_t rawWrite (const uint8_t*);

	inline void setOffsetMap (const uint8_t o, const uint8_t m) {
		_o = o;
		_m = m;
	}

	virtual inline void setCursor (const int x, const int y) {
		_x = x;
		_y = y;
	}

	virtual inline void moveCursor (const int x, const int y) {
		_x += x;
		_y += y;
	}

	virtual inline bool setContrust (const uint8_t v) {
		return sendTwoCommand(SSD1306_SET_CONTRAST, v);
	}

	virtual inline bool sleep (bool f = true) {
		_isSleep = f;
		return sendOneCommand(f ? SSD1306_DISPLAY_OFF : SSD1306_DISPLAY_ON);
	}

	virtual inline bool invert (bool f = true) {
		return sendOneCommand(f ? SSD1306_INVERT_DISPLAY : SSD1306_NORMAL_DISPLAY);
	}

	virtual inline bool mirror (bool f = true) {
		return sendOneCommand(f ? SSD1306_SET_SEGMENT_REMAP_HIGH : SSD1306_SET_SEGMENT_REMAP_LOW);
	}

	virtual inline bool flip (bool f = true) {
		return sendOneCommand(f ? SSD1306_SET_COM_SCAN_DEC : SSD1306_SET_COM_SCAN_INC);
	}

	virtual inline bool isSleep (void) {
		return _isSleep;
	}

	virtual inline bool vOffset (const int y) {
		return sendTwoCommand(SSD1306_SET_DISPLAY_OFFSET, y & 0x3F);
	}
};

class OLED_M096P : public OLED_SSD1306 {
public :
	using OLED_SSD1306::OLED_SSD1306;
	using super = OLED_SSD1306;
	OLED_M096P (void) {
		setOffsetMap(1, 0x12);
	}
};

class OLED_M091P : public OLED_SSD1306 {
public :
	using OLED_SSD1306::OLED_SSD1306;
	using super = OLED_SSD1306;
	OLED_M091P (void) {
		setOffsetMap(0, 2);
	}
};

#endif
