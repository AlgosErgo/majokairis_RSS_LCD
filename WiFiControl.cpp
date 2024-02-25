#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "WiFiControl.hpp"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

//String hostname = "ESP32E-8MB";

char* default_ssid = "ssid";
char* default_password = "password";

const char* ssidFileName = "/ssid.txt";
const char* passwordFileName = "/password.txt";
const int ChipSelect = 5;


//=======================================================================
//   WiFi control
//=======================================================================


bool wifi_isConnected(void) {
   if (WiFi.status() == WL_CONNECTED) {
      return true;
   } else {
      return false;
   }
}

bool SD_isConnected(void) {
   if (SD.begin(ChipSelect)) {
      return true;
   } else {
      return false;
   }
}

void wifiDisconnect(void) {
   delay(100);                   // wait for Wifi module
   WiFi.disconnect(true, true);  // おまじない
   delay(100);
   Serial.println("[WIFI] Disconnected!");
}


// [SDカード配線]
/*
* CardReader - ESP32
* GND - GND
* VCC - 5V
* MISO - 19
* MOSI - 23
* SCK - 18
* CS - 5
*/

/*
 * SDカードからSSIDとパスワードを取り出してwifi接続する関数
 */
void wifiConnect(void) {
   if (WiFi.status() != WL_CONNECTED) {
      String ssid;
      String password;

      // SDカードを接続。
      bool sd_card_status = false;
      int ct = 0;
      if (!SD.begin(ChipSelect)) {
         Serial.println("[SD] Error! Card Mount Failed. (code:01)");
         while (!SD.begin(ChipSelect)) {
            Serial.println(".");
            delay(1);
            ct++;
            if (ct > 2) {
               Serial.println("[SD] Time Out! (code:02)");
               sd_card_status = false;
               break;
            }
         }
      } else {
         // カードタイプの取得。
         uint8_t cardType = SD.cardType();
         if (cardType == CARD_NONE) {
            ct = 0;
            Serial.println("[SD] Error! No SD card attached! (code:03)");
            while (cardType == CARD_NONE) {
               Serial.println(".");
               delay(1);
               ct++;
               if (ct > 2) {
                  Serial.println("[SD] Time Out! (code:04)");
                  sd_card_status = false;
                  break;
               }
            }
         } else {
            // テキストファイルの読み込み
            File ssidFile = SD.open(ssidFileName);
            File passwordFile = SD.open(passwordFileName);

            if (!ssidFile || !passwordFile) {
               Serial.println("[SD] Error! Failed to open file. (code:05)");
               sd_card_status = false;
            }

            String ssid = ssidFile.readStringUntil('\n');
            String password = passwordFile.readStringUntil('\n');

            ssidFile.close();
            passwordFile.close();

            //文字列の先頭と末尾の空白を削除
            ssid.trim();
            password.trim();


            if (ssid == NULL || ssid == "") {
               Serial.println("[SD] Error! Read ssid.txt failed!! (code:06)");
               sd_card_status = false;
            } else if (password == NULL || password == "") {
               Serial.println("[SD] Error! Read password.txt failed!! (code:07)");
               sd_card_status = false;
            } else {
               Serial.println("[SD] Read file OK!");
               sd_card_status = true;
            }


            // WiFi接続を開始。

            delay(500);                   // wait for Wifi module
            WiFi.disconnect(true, true);  // おまじない
            delay(500);                   // wait for Wifi module

            if ((WiFi.getMode() != WIFI_STA)) {
               delay(100);
               WiFi.mode(WIFI_STA);
               delay(100);
            }

            //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
            //WiFi.setHostname(hostname.c_str());

            if (!sd_card_status) {
               Serial.println("[WIFI] Default SSID loaded. ");
               WiFi.begin(default_ssid, default_password);
            } else if (sd_card_status) {
               Serial.println("[WIFI] SSID loaded from SD card. ");
               WiFi.begin(ssid.c_str(), password.c_str());
            } else {
               Serial.println("[WIFI] bool:sd_card_status is NULL.");
               return;
            }


            Serial.println();
            Serial.print(F("[WIFI] Attempting to connect to SSID:"));
            Serial.print(ssid.c_str());
            Serial.print(F("/PASS:"));
            Serial.println(password.c_str());
            delay(500);  // wait for Wifi module


            bool online = true;
            ct = 0;
            while (WiFi.status() != WL_CONNECTED) {
               Serial.print(".");
               delay(500);
               ct++;
               if (ct > 60) {
                  online = false;
                  break;
               }
            }

            if (online == true) {
               Serial.println();
               Serial.print("[WIFI] Connected to ");
               Serial.println(ssid);
               Serial.println(WiFi.localIP());
               delay(100);
            } else {
               Serial.println();
               Serial.println("[WIFI] Error. Can't connect wifi. ");
               delay(100);
            }
         }
      }
   } else {
      Serial.println();
      Serial.print("[WIFI] Connected!!");
      Serial.println(WiFi.localIP());
      delay(100);
   }
}