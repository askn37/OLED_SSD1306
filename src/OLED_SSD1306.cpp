/***************
 *
 * OLED_SSD1306 - OLED SSD1306 (i2c) Interface for Arduino
 *
 * call constructor:
 *
 *  OLED_SSD1306
 *  OLED_M096P
 *  OLED_M091P
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/GPS_MTK333X
 * maintainer: askn https://twitter.com/askn37
 *
 */

#include <Arduino.h>
#include <OLED_SSD1306.h>
#include <FONT8X16MIN.h>

// ディスプレイ初期化コード
const PROGMEM uint8_t OLED_SSD1306_DISPLAY_INIT[] = {
    SSD1306_CONTROL_CMD_STREAM,         // control byte, Co bit = 0 (continue), D/C# = 0 (command)
    SSD1306_DISPLAY_OFF,                // display off
    SSD1306_SET_CHARGE_PUMP,            // Set Enable charge pump regulator 0x8D, 0x14
        0x14,
    SSD1306_SET_MULTIPLEX_RATIO,        // Set Multiplex Ratio  0xA8, 0x3F
        0x3F,                           // 64 MUX
    SSD1306_SET_DISPLAY_START_LINE,     // Set Display Start Line 0x40-0x7F
    SSD1306_SET_SEGMENT_REMAP_LOW,      // Set Segment re-map 0xA0/0xA1
    SSD1306_SET_COM_SCAN_INC,           // Set COM Output Scan Direction 0xC0/0xC8
    SSD1306_SET_CONTRAST,               // Set Contrast Control 0x81, 0x7F（明るさ設定）
        0x7F,                           // 0-255
    SSD1306_DISPLAY_ALL_ON_RESUME,      // Disable Entire Display On 0xA4
    SSD1306_NORMAL_DISPLAY,             // Set Normal Display 0xA6, Inverse display 0xA7
    SSD1306_SET_DISPLAY_CLK_DIV,        // Set Display Clock Divide Ratio/Oscillator Frequency 0xD5, 0x80
        0x80,
    SSD1306_DEACTIVATE_SCROLL,          // Deactivate scrollスクロール表示解除
    SSD1306_SET_MEMORY_ADDR_MODE,       // Set Memory Addressing Mode 0x20
        0x10,
    SSD1306_SET_COLUMN_RANGE,           // set Column Address 0x21
        0x00,                           // Column Start Address←水平開始位置はここで決める(0～127)
        0x7F,                           // Column Stop Address　画面をフルに使う
    SSD1306_SET_PAGE_RANGE,             // Set Page Address 0x22
        0x00,                           // 垂直開始位置（ページ）
        0x07                            // 垂直終了位置（ページ）
};

bool OLED_SSD1306::begin (const long i2cSpeed, const uint8_t i2caddr) {
    if (!_speed) Wire.begin();
    _addr = i2caddr;
    _isSleep = false;
    _speed = i2cSpeed;
    Wire.setClock(_speed);
    Wire.beginTransmission(_addr);
    uint8_t *p = (uint8_t*) OLED_SSD1306_DISPLAY_INIT;
    for (size_t i = 0; i < sizeof(OLED_SSD1306_DISPLAY_INIT); i++) {
        Wire.write(pgm_read_byte_near(p++));
    }
    Wire.write(SSD1306_SET_DISPLAY_OFFSET);
    Wire.write(_o);
    Wire.write(SSD1306_SET_COM_PIN_MAP);
    Wire.write(_m);
    Wire.write(SSD1306_DISPLAY_ON);                     // Display On 0xAF
    return (!Wire.endTransmission() && clear() );
}

size_t OLED_SSD1306::write (const uint8_t c) {
    if (c == '\r') return newLine(_y, _x);
    if (c == '\n') { _y++; return 1; }
    if (_y >= 4 || _x >= 16) return 1;
    if (_isSleep) return 0;
    uint8_t d = (c & 0x80) ? 0xFF : 0;
    uint8_t e = c & 0x7F;
    uint8_t x = _x << 3;
    uint8_t *p = (uint8_t *) FONT8X16MIN_CHARBITMAP;
    if (e > 0x20 && e < 0x80) p += (uint16_t)(e - 0x20) << 4;
    Wire.setClock(_speed);
    Wire.beginTransmission(_addr);
    Wire.write(SSD1306_CONTROL_CMD_STREAM);             // コマンド送信モード
    Wire.write((_y << 1) | 0xB0);                       // ページ指定：垂直位置
    Wire.write(SSD1306_SET_COLUMN_RANGE);               // アドレス指定
    Wire.write(x);                                      // 水平位置
    Wire.write(x + 7);                                  // 8バイトの範囲で折り返し
    Wire.endTransmission();
    Wire.beginTransmission(_addr);
    Wire.write(SSD1306_CONTROL_DATA_STREAM);            // 前述のアドレスにデータを16byte送信
    for (uint8_t i = 0; i < 16; i++) {
        Wire.write(d ^ pgm_read_byte_near(p++));
    }
    _x++;
    return !Wire.endTransmission();
}

size_t OLED_SSD1306::rawWrite (const uint8_t *q) {
    if (_y >= 4 || _x >= 16) return 1;
    if (_isSleep) return 0;
    uint8_t x = _x << 3;
    uint8_t *p = (uint8_t *) q;
    Wire.setClock(_speed);
    Wire.beginTransmission(_addr);
    Wire.write(SSD1306_CONTROL_CMD_STREAM);             // コマンド送信モード
    Wire.write((_y << 1) | 0xB0);                       // ページ指定：垂直位置
    Wire.write(SSD1306_SET_COLUMN_RANGE);               // アドレス指定
    Wire.write(x);                                      // 水平位置
    Wire.write(x + 7);                                  // 8バイトの範囲で折り返し
    Wire.endTransmission();
    Wire.beginTransmission(_addr);
    Wire.write(SSD1306_CONTROL_DATA_STREAM);            // 前述のアドレスにデータを16byte送信
    for (uint8_t i = 0; i < 16; i++) {
        Wire.write(*p);
        p++;
    }
    _x++;
    return !Wire.endTransmission();
}

bool OLED_SSD1306::clear (void) {
    if (_isSleep) return false;
    Wire.setClock(_speed);
    Wire.beginTransmission(_addr);
    Wire.write(SSD1306_CONTROL_CMD_STREAM);             // コマンド送信モード
    Wire.write(SSD1306_SET_PAGE_START_ADDRESS);         // ページ指定：垂直位置
    Wire.write(SSD1306_SET_COLUMN_RANGE);               // アドレス指定
    Wire.write(0x00);                                   // 水平位置先頭アドレス
    Wire.write(0x7F);                                   // 水平位置終了アドレス
    int f = !Wire.endTransmission();
    for (int i = 0; i < 64; i++) {                      // Wireは1度に32バイトまでしか送れないのでchunk化する
        if (!f) break;
        Wire.beginTransmission(_addr);
        Wire.write(SSD1306_CONTROL_DATA_STREAM);        // データ送信モード
        for (int j = 0; j < 16; j++) Wire.write(0U);
        f = !Wire.endTransmission();
    }
    _x = _y = 0U;
    return f;
}

// 行クリア
bool OLED_SSD1306::newLine (const int y, const int x) {
    _x = 0U;
    _y = y;
    if (_y >= 4 || x >= 16) return false;
    if (_isSleep) return false;
    Wire.setClock(_speed);
    Wire.beginTransmission(_addr);
    Wire.write(SSD1306_CONTROL_CMD_STREAM);             // コマンド送信モード
    Wire.write((_y << 1) | 0xB0);                       // ページ指定：垂直位置
    Wire.write(SSD1306_SET_COLUMN_RANGE);               // アドレス指定
    Wire.write(x << 3);                                 // 水平位置先頭アドレス
    Wire.write(0x7F);                                   // 水平位置終了アドレス
    int f = !Wire.endTransmission();
    if (f) {
        Wire.beginTransmission(_addr);
        Wire.write(SSD1306_CONTROL_DATA_STREAM);        // 前述のアドレスにデータを16byte送信
        for (int i = x; i < 16; i++) {                  // Wireは1度に32バイトまでしか送れないのでchunk化する
            if (!f) break;
            Wire.beginTransmission(_addr);
            Wire.write(SSD1306_CONTROL_DATA_STREAM);    // データ送信モード
            for (int j = 0; j < 16; j++) Wire.write(0U);
            f = !Wire.endTransmission();
        }
    }
    return f;
}

bool OLED_SSD1306::sendOneCommand (uint8_t c) {
    Wire.setClock(_speed);
    Wire.beginTransmission(_addr);
    Wire.write(SSD1306_CONTROL_CMD_SINGLE);             // control byte, Co bit = 1 (One command only), D/C# = 0 (command)
    Wire.write(c);
    return !Wire.endTransmission();
}

bool OLED_SSD1306::sendTwoCommand (uint8_t c, uint8_t d) {
    Wire.setClock(_speed);
    Wire.beginTransmission(_addr);
    Wire.write(SSD1306_CONTROL_CMD_STREAM);
    Wire.write(c);
    Wire.write(d);
    return !Wire.endTransmission();
}

// end of code


