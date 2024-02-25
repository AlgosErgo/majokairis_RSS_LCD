// ###########################################################
//
//      マジョカアイリス　Yahoo! News 電光表示板
//
//        V0.1  2021/01/11 by Kimio Ohe
//        V0.5  2021/01/13 スクロール右端改善
//        V1.0  2021/01/14 40dot Font Support
//        V1.1  2021/01/29 Refactoring
//        V1.2  2021/03/26 テキスト色不正を修正
//
//
//
// ###########################################################
//
//      マジョカアイリス 640*48px 細長LCD 動作テストコード
//
//      Hideto Kikuchi / PJ (@pcjpnet) - http://pc-jp.net/
//
// ###########################################################
//
//
//      Example Code for ESP32 Dev Module
//      Require LovyanGFX 0.3.8 Library
//
//
// ####################  LCD Pin Assignments  ####################
//
//  0.5mm Pitch - 22Pin
//
//  22   - [to GND]
//  21   - [to ESP32 - 32]     // ESP32 のGPIO32に対応（以下同様）
//  20   - [to GND]
//  19   - [to ESP32 - 12]
//  18   - [to ESP32 - 13]
//  17   - [to ESP32 - 26]
//  16   - [to ESP32 - 25]
//  15   - [to ESP32 - 21]     // 元IO17
//  14   - [to ESP32 - 22]     // 元IO16　32Eは未対応
//  13  -  [to ESP32 - 27]
//  12  -  [to ESP32 - 14]
//  11  -  [to GND]
//  10  -  [to ESP32 - 2]
//  9  -   [to ESP32 - 4]
//  8  -   [to ESP32 - 33]
//  7  -   [to ESP32 - 15]
//  6  -   [OPEN]
//  5  -   [to 3.3V]
//  4  -   [to 3.3V]
//  3  -   [to DCDC 5.5V~]
//  2  -   [to DCDC GND]
//  1  -   [to GND]
//

//################### NTP 実装資料 ##########################
//
// ESP32 の 時刻取得のオフライン時の挙動
//https://megalodon.jp/2022-1118-0251-40/https://qiita.com:443/yomori/items/0d72ab3d9556a0b7ef70
//
// ESP32 NTP 時計
//https://megalodon.jp/2022-1118-0252-40/https://blog.goo.ne.jp:443/jh7ubc/e/7bb47f44e733a859c0dd44b84bb878f1
//
// Wi-Fi接続とNTPクライアントの実験
//https://megalodon.jp/2022-1118-0254-12/https://garretlab.web.fc2.com:443/arduino/lab/ntp/
//
//
//


#include <Arduino.h>
#include <string.h>
#include "WiFiControl.hpp"
#include "YahooNewsLib.hpp"
#include "JmaWeatherLib.hpp"

#include <time.h>
#define JST 3600 * 9
static const char* wd[7] = { "日", "月", "火", "水", "木", "金", "土" };
struct tm timeInfo;
char time_str[32];
uint32_t WebGet_LastTime;

#include <LovyanGFX.hpp>
#define LCD_FREQ 15000000
//#define FONT &fonts::lgfxJapanMincho_40
#define FONT &fonts::lgfxJapanGothic_40
#define FONT_SIZE 48  // 40 or 48

//=======================================================================
//  LGFX Config
//=======================================================================
struct LGFX_Config {
   static constexpr int gpio_wr = 4;   //to LCD WR(14)
   static constexpr int gpio_rd = 2;   //to LCD RD(13)
   static constexpr int gpio_rs = 15;  //to LCD DC(16)
   static constexpr int gpio_d0 = 12;  //to LCD D0(4)
   static constexpr int gpio_d1 = 13;  //to LCD D1(5)
   static constexpr int gpio_d2 = 26;  //to LCD D2(6)
   static constexpr int gpio_d3 = 25;  //to LCD D3(7)
   static constexpr int gpio_d4 = 21;  //元IO17 -> 18 -> 21 to LCD D4(8)。IO17,16はESP32Eが未対応
   static constexpr int gpio_d5 = 22;  //元IO16 -> 19 -> 22 to LCD D5(9)。
   static constexpr int gpio_d6 = 27;  //to LCD D6(10)
   static constexpr int gpio_d7 = 14;  //to LCD D7(11)
};
static lgfx::LGFX_PARALLEL<LGFX_Config> lcd;
static lgfx::Panel_ILI9342 panel;
static lgfx::LGFX_Sprite buf;   // [640 * 48] Buffer
static lgfx::LGFX_Sprite cBuf;  // ex [40 * 40] Buffer

inline void lcd_buffer_write();

//=======================================================================
// RSS設定
//=======================================================================/

unsigned int RssChanel = 0;  // トピックス選択
const char* RssLines[9] = {
   "top-picks",
   "domestic",
   "world",
   "science",
   "entertainment",
   "sports",
   "local",
   "business"
   "it"
};

unsigned int ct = 0;   // 天気予報とニュースを交互に表示させるためのカウンタ
bool SD_check = true;  // 初回起動時にSDカードをチェック
String str;            // LCDに表示する文字列


//=======================================================================
//  ボタン割り込み
//=======================================================================

#define BUTTON_1_PIN 16
#define BUTTON_2_PIN 17
volatile bool flag_button_pushed = false;  // 排他用処理中フラグ
volatile ulong time_button_pushed = 0;     // 操作時刻保存用
int last_button = -1;                      // 直前に押されたボタン
volatile int button_pressed;
int button_1_ct = 1;          // 天気予報切換ボタン
int button_2_ct = 0;          // yahooニュース切換ボタン

// 割り込み処理 ボタンを検知
void IRAM_ATTR button_pushed() {
   if (!flag_button_pushed) {
      /*条件演算子（3項演算子）
      条件演算子を使うとif-else文のような条件分岐を実現することができます。
      条件演算子は「?」記号と「:」記号を使って、次のように記述します。
      (e.g.)式1 ? 式2 : 式3
      式1が真であれば → 式2を実行
      式1が偽であれば → 式3を実行*/
      button_pressed = digitalRead(BUTTON_1_PIN) == LOW ? 1 : (digitalRead(BUTTON_2_PIN) == LOW ? 2 : -1);

      if (button_pressed != -1) {
         time_button_pushed = millis();
         last_button = button_pressed;
         // ボタンごとの処理をここに追加
         if (button_pressed == 1) {
            flag_button_pushed = true;
            Serial.println("BUTTON_1が押されたときの処理");
         } else if (button_pressed == 2) {
            flag_button_pushed = true;
            Serial.println("BUTTON_2が押されたときの処理");
         }
      }
   }
}

//ESP32でプルアップとGPIO割り込み（Interrupt）をテストする
//https://megalodon.jp/2023-1121-1953-33/https://labo.mycabin.net:443/electronics-programming/arduino-esp32/781/


//=======================================================================
//  setup()
//=======================================================================
void arduino_setup() {
   Serial.begin(115200);

   WebGet_LastTime = 200000;  //起動時に記事をGETするために、多めの数値で初期化しておく

   //=====================================================================
   //  Setup Interrupt
   //=====================================================================
   // GPIOピンモードを設定
   pinMode(BUTTON_1_PIN, INPUT);
   pinMode(BUTTON_2_PIN, INPUT);

   // 押されたフラグを初期化
   flag_button_pushed = false;
   // 押された時刻を初期化
   time_button_pushed = millis();

   // 割り込みを登録 トリガはLOWになった時
   attachInterrupt(BUTTON_1_PIN, button_pushed, FALLING);
   attachInterrupt(BUTTON_2_PIN, button_pushed, FALLING);

   //=====================================================================
   //  Setup LCD
   //=====================================================================
   panel.freq_write = LCD_FREQ;  // WiFiと同居時は8MHzに設定
   panel.freq_fill = 27000000;
   panel.freq_read = 16000000;
   panel.len_dummy_read_pixel = 8;

   panel.spi_cs = 33;  //to LCD CS(15)
   panel.spi_dc = -1;
   panel.gpio_rst = 32;  //to LCD RST(2)

   panel.gpio_bl = -1;
   panel.pwm_ch_bl = -1;
   panel.backlight_level = true;

   panel.memory_width = 320;
   panel.memory_height = 240;

   panel.panel_width = 320;
   panel.panel_height = 96;

   panel.offset_x = 0;
   panel.offset_y = 144;

   panel.rotation = 0;
   panel.offset_rotation = 0;

   lcd.setPanel(&panel);

   lcd.init();
   lcd.setColorDepth(16);

   lcd.setRotation(0);  // 0 or 2

   buf.setColorDepth(16);
   buf.createSprite(640, 48);
   cBuf.createSprite(FONT_SIZE, FONT_SIZE);
   //  buf.setFont(&fonts::lgfxJapanGothic_40);        // 40ドットフォント設定
   cBuf.setFont(FONT);  // 40ドットフォント設定
   if (FONT_SIZE == 48) {
      cBuf.setTextSize(1.2, 1.2);
   }

   // 登録色一覧           //   R,   G,   B
   //BLACK       = 0x0000; //   0,   0,   0
   //NAVY        = 0x000F; //   0,   0, 128
   //DARKGREEN   = 0x03E0; //   0, 128,   0
   //DARKCYAN    = 0x03EF; //   0, 128, 128
   //MAROON      = 0x7800; // 128,   0,   0
   //PURPLE      = 0x780F; // 128,   0, 128
   //OLIVE       = 0x7BE0; // 128, 128,   0
   //LIGHTGREY   = 0xC618; // 192, 192, 192
   //DARKGREY    = 0x7BEF; // 128, 128, 128
   //BLUE        = 0x001F; //   0,   0, 255
   //GREEN       = 0x07E0; //   0, 255,   0
   //CYAN        = 0x07FF; //   0, 255, 255
   //RED         = 0xF800; // 255,   0,   0
   //MAGENTA     = 0xF81F; // 255,   0, 255
   //YELLOW      = 0xFFE0; // 255, 255,   0
   //WHITE       = 0xFFFF; // 255, 255, 255
   //ORANGE      = 0xFD20; // 255, 165,   0
   //GREENYELLOW = 0xAFE5; // 173, 255,  47
   //PINK        = 0xF81F; // 255,   0, 255

   //cBuf.setTextColor(TFT_WHITE, TFT_BLACK);
   cBuf.setTextColor(TFT_WHITE, 0x000F);
   cBuf.setTextWrap(false);
   buf.setScrollRect(0, (48 - FONT_SIZE) / 2, 640, FONT_SIZE);  // スクロール幅設定
}

//=======================================================================
//  main loop()
//=======================================================================
void arduino_loop() {

   wifiConnect();

   //=====================================================================
   // 15秒毎に巡回
   //=====================================================================
   if ((millis() - WebGet_LastTime) > 15000 && flag_button_pushed == false) {
      if (wifi_isConnected() == true) {
         configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
         getLocalTime(&timeInfo);
         sprintf(time_str, "%02d/%02d(%s) %02d:%02d       ",
                 timeInfo.tm_mon + 1, timeInfo.tm_mday, wd[timeInfo.tm_wday],
                 timeInfo.tm_hour, timeInfo.tm_min);

         str = time_str;

         if (ct % 2 == 0) {
            str += "            ◆ 千葉 の天気                  ";
            str += weather_Web_Get(timeInfo.tm_hour, 0);
            str += "                 ";
            str += weather_Web_Get(timeInfo.tm_hour, 1);

         } else {
            if (RssChanel >= 8) {
               RssChanel = 0;
            }
            String URL = String(RssLines[RssChanel]);

            str += https_Web_Get("news.yahoo.co.jp", "/rss/topics/" + URL + ".xml", '\n', "</rss>",
                                 "<title>", "</title>", "<description>", "</description>", "    ◇");
            //    str = https_Web_Get("news.yahoo.co.jp", "/rss/topics/"+URL+".xml", '\n', "</rss>", "<title>", "</title>", "<description>", "</description>", " ◇");
            //    str =  + str + "　　　　　　　　　　　　　　　　　　　";
            RssChanel++;
         }

         ct++;
         //wifiDisconnect();
         WebGet_LastTime = millis();
         str += "　　　　　　　　　　　　　　　　　";


      } else if (SD_check) {
         if (SD_isConnected() == false) {
            Serial.println("SD_isConnected()==false");
            str = "[Error] SDカードの読み込みに失敗    ";
            SD_check = false;
         }
      } else {
         Serial.println("YahooNewsEX : wifi_isConnected()==False");
         str = "[Error] Wifi接続に失敗。     ";
      }

      //=====================================================================
      // Newsをスクロール表示
      //=====================================================================
      int32_t pStr = 0;
      int32_t cursor_x = 0;

      cBuf.setCursor(cursor_x, 0);  // カーソル位置を初期化
      while (str[pStr] != '\0') {
         // マルチバイト文字1文字を取り出し1文字バッファに表示
         cBuf.print(str[pStr]);         // 1バイトずつ出力 (マルチバイト文字でもこの処理で動作します)
         cursor_x = cBuf.getCursorX();  // 出力後のカーソル位置を取得
         pStr++;
         // 細長LCDの右端に1文字分表示
         if (cursor_x != 0) {                                         // 表示する文字があるか？
            for (int i = 0; i < cursor_x; i++) {                      // 文字幅分スクロール表示
               buf.scroll(-1, 0);                                     // 1ドットスクロール
               cBuf.pushSprite(&buf, 639 - i, (48 - FONT_SIZE) / 2);  // 1文字バッファをbuf右端に転送
               lcd_buffer_write();                                    // LCDへデータ転送
            }
         }

         //=====================================================================
         // 割り込み処理
         //=====================================================================

         /*if (flag_button_pushed && (millis() - time_button_pushed) > 50) {
         // ボタンが押された後の処理をここに追加
         flag_button_pushed = false;
         time_button_pushed = millis(); // フラグと時刻をリセット
         }*/

         if (flag_button_pushed) {
            delay(200);
            pStr = 0;
            cursor_x = 0;
            str = "             ";
            if (button_pressed == 1) {
               if (button_1_ct == 1) {
                  str += weather_Web_Get(timeInfo.tm_hour, 0);
                  button_1_ct++;
               } else {
                  str += weather_Web_Get(timeInfo.tm_hour, 1);
                  button_1_ct = 1;
               }
               Serial.println("[button1] 割り込み完了");
               flag_button_pushed = false;

            } else if (button_pressed == 2) {
               if (button_2_ct >= 9) {
                  button_2_ct = 0;
               }
               String URL = String(RssLines[button_2_ct]);
               str += https_Web_Get("news.yahoo.co.jp", "/rss/topics/" + URL + ".xml", '\n', "</rss>",
                                    "<title>", "</title>", "<description>", "</description>", "    ◇");
               button_2_ct++;
               Serial.println("[button2] 割り込み完了");
               flag_button_pushed = false;
            }
         }
         // 次の文字表示の準備
         cursor_x = 0;
         cBuf.setCursor(cursor_x, 0);  // カーソル位置を初期化
      }
   }
}

//=======================================================================
//  640*48px Buffer to 320*96px LCD
//=======================================================================
inline void lcd_buffer_write() {
   lcd.startWrite();
   lcd.setAddrWindow(0, 0, 320, 96);
   lcd.writePixels((lgfx::swap565_t*)buf.getBuffer(), 640 * 48);
   lcd.endWrite();
}
