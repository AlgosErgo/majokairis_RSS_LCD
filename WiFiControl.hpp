#ifndef WIFICONTROL_hpp
#define WIFICONTROL_hpp

bool wifi_isConnected(void);
bool SD_isConnected(void);
void wifiDisconnect(void);
void wifiConnect(void);


#endif


/* ESP32でmicroSDカードを読み書きしてみました（おおたfab 電子工作初心者勉強会）
*  https://megalodon.jp/2023-1019-2246-34/https://kanpapa.com:443/today/2023/01/esp32-otafab-study-microsd.html
*/

/*【ESP32】SSID、パスワードをSDカードに入れたファイルから読み込んでWiFi接続を行う
*  https://megalodon.jp/2023-1019-0052-36/https:/rikoubou.hatenablog.com:443/entry/2017/09/13/154258
*/


/* RTC(I2C接続)、SDカード(SPI接続)、アナログセンサーをESP32-WROOM-32Eにつなぐ
*  https://megalodon.jp/2023-1018-2007-11/https://k-kuro.hatenadiary.jp:443/entry/20221104/1667552008
*/

/*  //ESP32にSmartConfigでWiFi情報を伝える
*   https://megalodon.jp/2023-1104-1138-14/https://www.kerislab.jp:443/posts/2018-02-16-esp32-smartconfig/
*/






