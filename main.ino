// ###########################################################
//
//      マジョカアイリス　Yahoo! News 電光表示板 EX11
//
//        V0.1  2021/01/11 by Kimio Ohe
//        V0.5  2021/01/13 スクロール右端改善
//        V1.0  2021/01/14 40dot Font Support
//        V1.1  2021/01/29 Refactoring
//
//        V1.2  RSS URL追加
//        V1.3  Weather追加 wifiを分離
//
//        V1.4   2022/11/18 時刻表示追加
//        V1.4.1 2022/12/03 wificontrolをライブラリ化、privateからSSID PASSを抽出し、天気予報assemble準備
//        V1.5   2022/12/21 jma web getをアセンブル 
//        V1.5.1 2022/12/22 リアルタイム表示、mainの可読性向上
//        V1.6   2023/10/19 wifi設定読み込み用のSDカードを実装。ピン番号を変更。
//                        [変更]
//                          ・YahooNewsEx11.cpp
//                          ・WiFiControl.cpp
//        V1.7   2023/11/23 ボタン割り込みを実装。wifi接続前にコネクションを確認 
//                        [変更]
//                          ・YahooNewsEx13.cpp
//                          ・WiFiControl.cpp
//        V1.8   2023/11/30 ボタン割り込み完成。ニュース・トピックスを削除
//                        [変更]
//                          ・YahooNewsEx14.cpp
//                          ・YahooNewsLib

// ###########################################################

void arduino_setup(void);
void arduino_loop(void);

void setup(void) {
    arduino_setup();
}

void loop(void) {
    arduino_loop();
}
