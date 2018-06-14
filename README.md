# OLED_SSD1306

OLED : SOLOMON SYSTECH SSD1306 (I2C) small driver for Arduino

I2C接続の OLEDをキャラクタLCD ライクに使用するための軽量実装。

## 概要

このクラスライブラリは
[SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
を使用した I2C接続の 128x64〜128x32 ピクセルの廉価な 単色グラフィックOLEDを、
キャラクタLCDのように扱えるようにしたものである。
この制御チップは
[U8glib](https://github.com/olikraus/u8glib)
等でひろくサポートされいるが、単なる英数文字表示を行いたいだけの場合には
実装サイズが非常に大きく、かつ動作が遅すぎる。

このクラスライブラリは、以下の指針に基づいて設計した。

- チップが有しているSPIおよびパラレルIFには対応せず、I2C接続のみとする。
- 8x16ピクセル英数キャラクタ表示に特化する。
さらに省サイズ化のためフォントデータは1種類のみ用意し、
かつ 0x20〜0x7F の96キャラクタに限定する。
- 改行に関わる "\r" "\n" には対応し、println() を使用可能にする。
- カーソル表示、および点滅表示はサポートしない。
しかしながら未使用の MSBを用いて反転文字表示には対応する。
- 別途漢字表示等の応用を可能とするため、バイト配列でのデータ書込メソッドを設ける。

動作検証および対応製品は
aitendo
[M096P](http://www.aitendo.com/product/14958)(128x64)
および
[M091P](http://www.aitendo.com/product/15721)(128x32)
とした。

<img src="https://www.aitendo.com/data/aitendo/product/20161113_7ccffa.png" width="320">
<img src="http://aitendo3.sakura.ne.jp/aitendo_data/product_img/lcd/oled/0.91/0.91.png" width="240">

## Arduino IDE への導入

1. .ZIPアーカイブをダウンロードする。[Click here](https://github.com/askn37/OLED_SSD1306/archive/master.zip)

2. ライブラリマネージャで読み込む

    スケッチ -> ライブラリをインクルード -> .ZIP形式のライブラリをインストール...

## とっかかり

結線は、OLED と Arduino (UNO) の VCC(+5V)と GNDのほか、
SCL(A5) 同士、SDA(A4) 同士を繋ぐだけである。

```c
#include <Arduino.h>
#include "OLED_SSD1306.h"

OLED_M096P oled;
// OLED_M091P oled;

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
```

## リファレンス

### OLED_SSD1306 (void)
### OLED_M096P (void)
### OLED_M091P (void)

コンストラクタは、対応する製品型番別にある。

OLED_SSD1306 は基礎クラスであり、
128x64 ピクセルのノーマルな OLEDパネルを想定して
もっともプレーンと思われる設定を初期値とする。

OLED_M096P は、128x64 ピクセルの OLEDパネルを使用するが
表示開始位置が1ラインずれているのに対応している。

OLED_M091P は、128x32 ピクセルの OLEDパネルを使用する。
表示可能な情報量は半分となる。

### bool begin (const long speed = I2C\_SPEED\_FAST, const uint8\_t i2caddr = SSD1306\_ADDR)

第1引数指定の速度でI2Cバスを初期化し、
第2引数指定のアドレスでデバイスとの通信を開始する。
成功すれば真を返す。

第1引数の省略値は I2C\_SPEED\_FAST（400Kbps）、
第2引数の省略値は SSD1306\_ADDR（0x3C）である。

### void setClock (long speed)

第1引数の指定値でI2Cバス速度を変更する。

### bool clear (void)

画面全部をクリアし、カーソル位置を (0,0) == 左上原点に移動する。
成功すれば真を返す。
begin() メソッド内からは暗黙的に呼ばれる。

### void setCursor (const int x, const int y) 

カーソル位置を指定した絶対座標に移動する。
x桁（横軸）の有効範囲は 0〜15、
y行（縦軸）の有効範囲は 0〜3で、それ以外の値は画面外となる。

M091Pのような 128x32ピクセルパネルの場合は
上半分だけが表示されている状態であるため、
y行（縦軸）の有効範囲は 0か 1となるが、
flip() メソッドで上下反転している場合は、2か 3となる。

### void moveCursor (const int x, const int y)

カーソル位置を現在位置から相対指定で移動する。

### bool newLine (const int y, const int x = 0)

第1引数で指定した y行の、x桁から右をクリアし、カーソルを（x,y）に移動する。
成功すれば真を返す。
y行指定は省略できない。
x桁指定の省略値は 0である。

### size_t write (const uint8\_t)

指定のキャラクタを現在のカーソル位置に書き込む。
成功すれば真を返す。
カーソルは右に1桁（x + 1）移動する。

ただし '\r' が指定されるとカーソルは左端（x = 0）に移動し、
'\n' が指定されるとカーソルは次行の左端（x = 0, y + 1）に移動する。
その他の制御キャラクタは無視される。

表示できるキャラクタは 0x20〜0x7F の範囲だけである。

### size\_t print (...)
### size\_t println (...)

本クラスは Print クラスを継承しているので、
print() および println() を使用できる。

### bool setContrust (const uint8\_t bright)

点灯ピクセルの明るさを指定値に設定する。
成功すれば真を返す。
指定可能範囲は 0（消灯）〜 255（最大輝度）である。
輝度を上げるほど、かつ点灯しているピクセル数が多いほど、消費電力は増大する。

### bool sleep (bool flag = true)

真を渡すと OLEDパネルを消灯する。
偽を渡せば消灯解除される。
引数省略時は消灯である。
何れにせよ成功すれば真を返す。

消灯中は write() メソッド等は機能せず、偽を返す。

### bool isSleep (void) 

sleep() メソッドの設定状態を真偽値で返す。

### bool invert (bool flag = true) 

真を渡すと OLEDパネルの「明暗点灯」を反転する。
偽を渡すと反転状態は解除される。
引数省略時は反転である。
何れにせよ成功すれば真を返す。

### bool mirror (bool flag = true) 

真を渡すと OLEDパネルの「左右表示」を反転する。
偽を渡すと反転状態は解除される。
引数省略時は反転である。
何れにせよ成功すれば真を返す。

### bool flip (bool flag = true) 

真を渡すと OLEDパネルの「上下表示」を反転する。
偽を渡すと反転状態は解除される。
引数省略時は反転である。
何れにせよ成功すれば真を返す。

M091Pのような 128x32ピクセルパネルの場合は
上半分だけが表示されている状態であるため、
y行（縦軸）の有効範囲は 0か 1となるが、
flip() メソッドで上下反転している場合は、2か 3となる。

### void setOffsetMap (const uint8_t o, const uint8_t m)

使用する OLEDパネルのオフセットマッピング値を設定する。
begin() メソッド実行前に使わなければならない。
このメソッドはコンストラクタ内で使用される。

### bool vOffset (const int y)

OLEDパネルの表示開始ラインを変更する。
設定可能範囲は 0〜63 である。
成功すれば真を返す。

画面の上下は連続しているので、上下にはみだした表示は反対側に現れる。

### size\_t rawWrite (const uint8\_t* bitmaparray)

8x16ピクセルを表すビットマップを収めた 16バイト配列のポインタを指定し、
現在のカーソル位置に直接書き込む。
成功すれば真を返す。
カーソルは右に1桁進む。

ビットマップは縦8ピクセルが上端をMSBとした1バイトとし、
これを右方向へ8バイト並べた 8x8ピクセル表示を、
下方向へ 2組 == 16バイト並べた配列となっている。

このメソッドは主に漢字ROMを用いた全角キャラクタ表示に使用する。
16x16ピクセルのキャラクタを書き込む場合は、左半分、右半分の 2回に分けて使用する。

## 既知の不具合／制約／課題

- 主要な AVR 以外はテストされていない。
- 古い Arduino IDE には対応しない。1.8.5で動作確認。少なくとも C++11 は使えなければならない。
- 英文マニュアルが未整備である。

## 改版履歴

- 0.1.1

## 使用許諾

MIT

## 著作表示

朝日薫 / askn
(SenseWay Inc.)
Twitter: [@askn37](https://twitter.com/askn37)
GitHub: https://github.com/askn37

