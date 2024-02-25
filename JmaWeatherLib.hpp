//##############
//# [更新履歴]
// ##############
//#
//# 01 開発開始
//     
//# 02 HTTP GET成功
// 
//# 03 Arduino jsonによる抽出を開始
//
//# 04 back up
//
//# 05 web getをstring からchar に変更
//      httpget (String型) -> char型変換 -> arduinojsonで抽出 -> Stringに戻してreturn
//        非効率的すぎるので没に　
//
//# 06 arduinnojasonのstreamget に変更
//      How to use ArduinoJson with HTTPClient?
//      https://megalodon.jp/2022-1214-1122-31/https://arduinojson.org:443/v6/how-to/use-arduinojson-with-httpclient/
//      
//      構造体をreturnは却下（ライブラリ内と呼び出し元に二重の構造体が必要になる）
//
//      webget -> arduinojason -> String変換 -> 1つの文に纏めてからStringをReturn
//      jmaライブラリ内で構造体を使用する
//      ライブラリ内の構造体をキャッシュする
//      呼び出し元の引数で返すデータを変更する
//      today = weather_Web_Get(today_weather); 今日の天気
//      
//# 07 パースした要素のreturnに成功
//
//     
//　　　　構造体を作成してキャッシュする
//       一時間毎に再取得、それまで構造体の中身を保持
//      構造体はmainで実装すべき？
//
//# 08 nestされてたdetailのgetに成功
//
//　　　　tempとchanceofrainのパースに成功
//        
//      
//# 09 jmalibの最適化
//
//　　　　null判定を追加  
//        気温と降水確率をtodayとtomorrowで分岐
//　　　　明日の降水率がnullになる問題を修正
//        datelabelに日付を追加
//        wifi controlにタイムアウトとオンライン判定を実装
//      



String weather_Web_Get(int req_hour, int req_date);




/*String weather_Web_Get(const char* host, String City_Code,
                       char char_tag, String Final_tag,
                       String Begin_tag, String End_tag, String Begin_tag2,
                       String End_tag2, String Paragraph);*/

/*String weather_Web_Get(const char* host, String city_code,);*/



//String extract_Json(String payload, String target);

// const char* jp_weather_host = "weather.tsukumijima.net";
// String jp_weather_area_url = "/api/forecast/city/120010";
//https://weather.tsukumijima.net/api/forecast/city/120010


/*const int port = 443;
const char* jp_weather_host = "weather.tsukumijima.net";
String area_code_str = "120010"; //千葉
String office_code= "120000"; //千葉県
String jp_weather_area_url = "/api/forecast/city/" + area_code_str;*/




//https://weather.tsukumijima.net/
//https://weather.tsukumijima.net/primary_area.xml
//https://weather.tsukumijima.net/api/forecast/city/120010
//https://blog.tsukumijima.net/article/weather-api/
//https://typewriter.hatenablog.jp/entry/2017/10/31/230914
//https://kohacraft.com/archives/201910091646.html

//気象庁天気予報JSONを取得して天気予報を表示させてみた
//  https://megalodon.jp/2022-1202-1025-33/https://www.mgo-tec.com:443/blog-entry-jp-weather01.html/2

//【Arduino/ESP8266】HttpClientを使ってHTTPをGETする
//  https://megalodon.jp/2022-1207-1426-26/https://kurobekoblog.com:443/httpclient

//HTTPClientライブラリリファレンス
//  https://megalodon.jp/2022-1207-1427-32/https://garretlab.web.fc2.com:443/arduino/esp32/reference/libraries/HTTPClient/



//基礎で参考になった。arduino jason 使い方(失敗)
//M5Stackでwifiに接続し天気予報JSONを取得する
//  https://megalodon.jp/2022-1207-1429-30/https://yamaccu.github.io:443/tils/20220808-M5Stack-JSON

//jsonじゃないかった
//ArduinoでXMLを解析する  2009年8月25日
//http://web.archive.org/web/20221222165452/http://todotani.cocolog-nifty.com/blog/2009/08/arduinoxml-8671.html

//失敗
//ESP32 Arduinoで Ambientからデータを取得して得られたJSONをパースして値を抽出する方法
//  https://megalodon.jp/2022-1210-0744-29/https://kohacraft.com:443/archives/201910091646.html

//arduino jsonわかりやすかった
//ArduinoでJSONを扱う「ArduinoJSON」
//  https://megalodon.jp/2022-1209-0542-26/https://shuzo-kino.hateblo.jp:443/entry/2016/05/06/203603

//WiFiClientSecure でなく HTTPClient を使って M5Stack から HTTPS で AirTable にデータを書き込むメモ
//  https://megalodon.jp/2022-1207-1431-17/https://www.1ft-seabass.jp:443/memo/2022/05/14/m5stack-airtable-api-connect-using-httpclient/

//arduinojsonでString型のまま抽出する方法
//StringExample.ino
//  https://megalodon.jp/2022-1210-0353-39/https://arduinojson.org:443/v6/example/string/

//
//【Arduino】Arduino環境下でJSONをパース(逆シリアル化)して値を取り出してみる
//  https://megalodon.jp/2022-1210-0747-10/https://kurobekoblog.com:443/json

// 抽出コード自動生成
//　arduinojson　公式サイト
//https://arduinojson.org/v6/assistant/#/step1

//
//Getting json keys from an array from an object using ArduinoJson
//https://megalodon.jp/2022-1222-1716-05/https://stackoverflow.com:443/questions/71023794/getting-json-keys-from-an-array-from-an-object-using-arduinojson

//
//M5Stackで作る！最先端スマート洗濯バサミ【第3回】
//https://megalodon.jp/2022-1211-0510-00/https://deviceplus.jp:443/mc-general/smart-clothespin-with-m5stack-03/

// nestされたarrayの抽出方法 -> doc["forecasts"][0]["telop"];
//ESP8266(Arduino)でJSONを取得して、パースする
//https://megalodon.jp/2022-1219-2346-01/https://tekitoh-memdhoi.info:443/views/825

// nestされたobjectのパース方法　filter["list"][0]["main"]["temp"] = true;
//ArduinoJSON: Filtering Data
//https://megalodon.jp/2022-1221-0122-26/https://www.tutorialspoint.com:443/arduinojson-filtering-data

