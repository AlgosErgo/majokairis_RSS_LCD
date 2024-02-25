// jma lib ver10

#include <Arduino.h>
#include <string.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "JmaWeatherLib.hpp"


String url = "https://weather.tsukumijima.net/api/forecast/city/120010";
//https://www.jma.go.jp/bosai/forecast/data/forecast/120000.json

const char* weather_root_ca =
  "-----BEGIN CERTIFICATE-----\n"
  "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\n"
  "bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n"
  "-----END CERTIFICATE-----\n";



String weather_Web_Get(int req_hour, int req_date) {

  HTTPClient http;
  http.useHTTP10(true);  //おまじない

  String ret_str;

  Serial.println(F("[HTTP] jma begin...\n"));

  if (http.begin(url, weather_root_ca)) {
    int httpCode = http.GET();
    Serial.println(F("[HTTP] HTTP GET Send!\n"));

    if (httpCode > 0) {
      // # HTTP header has been send and Server response header has been handled
      Serial.println("[HTTP] GET... code:");
      Serial.print(httpCode);

      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        // # 接続に成功した場合
        Serial.print(F(" OK!!\n"));
        Serial.println(F("[HTTP] GET String Size:"));
        int Size = http.getSize();
        Serial.print(Size);

        DynamicJsonDocument doc(4096);
        StaticJsonDocument<512> filter;
        //取得するJSONキーを全てフィルターに追加する。
        filter["publishingOffice"] = true;
        filter["title"] = true;
        filter["forecasts"][0]["date"] = true;
        filter["forecasts"][0]["dateLabel"] = true;
        filter["forecasts"][0]["telop"] = true;
        filter["forecasts"][0]["detail"]["weather"] = true;
        filter["forecasts"][0]["temperature"]["min"]["celsius"] = true;
        filter["forecasts"][0]["temperature"]["max"]["celsius"] = true;
        filter["forecasts"][0]["chanceOfRain"]["T00_06"] = true;
        filter["forecasts"][0]["chanceOfRain"]["T06_12"] = true;
        filter["forecasts"][0]["chanceOfRain"]["T12_18"] = true;
        filter["forecasts"][0]["chanceOfRain"]["T18_24"] = true;

        DeserializationError error = deserializeJson(doc, http.getStream(),
                                                     DeserializationOption::Filter(filter),
                                                     DeserializationOption::NestingLimit(10));

        Serial.println(F("\n[JSON] Start Deserialization!"));

        bool parse_flag;
        switch (error.code()) {
          case DeserializationError::Ok:
            Serial.println(F(" - [OK!] Deserialize Success!!"));
            parse_flag = true;
            break;
          case DeserializationError::InvalidInput:
            Serial.println(F(" - [ERROR] Invalid input!"));
            parse_flag = false;
            break;
          case DeserializationError::NoMemory:
            Serial.println(F(" - [ERROR] Not enough memory"));
            parse_flag = false;
            break;
          default:
            Serial.println(F(" - [ERROR] Deserialization failed"));
            parse_flag = false;
            break;
        }

        if (parse_flag == true) {

          Serial.println(F("[JSON] Start parsing!"));
          String date = doc["forecasts"][req_date]["date"].as<String>();
          date = date.substring(8);
          ret_str = doc["forecasts"][req_date]["dateLabel"].as<String>() + "(" + date + "日)";
          ret_str += " :【 ";
          ret_str += doc["forecasts"][req_date]["telop"].as<String>();
          ret_str += " 】 ";
          //ret_str += "     詳細 : [ ";
          //ret_str += doc["forecasts"][req_date]["detail"]["weather"].as<String>();
          //ret_str += " ]";

          // req_date 0 : today
          // req_date 1 : tomorrow
          // req_date 2 : overmorrow
          //　[今日-明日 date] [天気 telop] [詳細 detail_weather] [気温 max/min] [降水確率6時間毎 chanceOfRain]

          if (req_date == 0 && req_hour <= 16) {
            ret_str += "                   ";
            ret_str += doc["forecasts"][req_date]["dateLabel"].as<String>();
            ret_str += "の気温 : ";
            ret_str += doc["forecasts"][req_date]["temperature"]["max"]["celsius"].as<String>();
            ret_str += "℃";
            String min_temp = doc["forecasts"][req_date]["temperature"]["min"]["celsius"].as<String>();

            if (min_temp != "null") {
              ret_str += "/";
              ret_str += min_temp;
              ret_str += "℃ ";
            }
          }

          ret_str += "                   ";

          if (req_date == 0 && req_hour <= 3) {
            // 0-5
            ret_str += doc["forecasts"][req_date]["dateLabel"].as<String>();
            ret_str += "の降水率(6時まで) : ";
            ret_str += doc["forecasts"][req_date]["chanceOfRain"]["T00_06"].as<String>();

          } else if (req_date == 0 && req_hour <= 11) {
            // 6-11
            ret_str += doc["forecasts"][req_date]["dateLabel"].as<String>();
            ret_str += "の降水率(12時まで) : ";
            ret_str += doc["forecasts"][req_date]["chanceOfRain"]["T06_12"].as<String>();

          } else if (req_date == 0 && req_hour <= 17) {
            //12-16
            ret_str += doc["forecasts"][req_date]["dateLabel"].as<String>();
            ret_str += "の降水率(18時まで) : ";
            ret_str += doc["forecasts"][req_date]["chanceOfRain"]["T12_18"].as<String>();

          } else if (req_date == 0 && req_hour <= 23) {
            //17-23
            String cOR = doc["forecasts"][req_date]["chanceOfRain"]["T18_24"].as<String>();
            if (cOR != "null") {
              ret_str += doc["forecasts"][req_date]["dateLabel"].as<String>();
              ret_str += "の降水率(23時まで) : ";
              ret_str += doc["forecasts"][req_date]["chanceOfRain"]["T18_24"].as<String>();
            }
          }

          if (req_date == 1) {

            String max_temp = doc["forecasts"][req_date]["temperature"]["min"]["celsius"].as<String>();

            if (max_temp != "null") {
              ret_str += "                 ";
              ret_str += doc["forecasts"][req_date]["dateLabel"].as<String>();
              ret_str += "の気温 : ";
              ret_str += doc["forecasts"][req_date]["temperature"]["max"]["celsius"].as<String>();
              ret_str += "℃";

              String min_temp = doc["forecasts"][req_date]["temperature"]["min"]["celsius"].as<String>();

              if (min_temp != "null") {
                ret_str += "/";
                ret_str += min_temp;
                ret_str += "℃ ";
              }
            }

            ret_str += "                    ";
            ret_str += doc["forecasts"][req_date]["dateLabel"].as<String>();
            ret_str += "の降水率: ";

            String cor1 = doc["forecasts"][req_date]["chanceOfRain"]["T00_06"].as<String>();
            String cor2 = doc["forecasts"][req_date]["chanceOfRain"]["T06_12"].as<String>();
            String cor3 = doc["forecasts"][req_date]["chanceOfRain"]["T12_18"].as<String>();
            String cor4 = doc["forecasts"][req_date]["chanceOfRain"]["T18_24"].as<String>();

            if (cor1 == cor2 && cor1 == cor3 && cor1 == cor4) {
              ret_str += cor1;
            } else {
              ret_str += "(6時毎) : ";
              ret_str += cor1;
              ret_str += "/";
              ret_str += cor2;
              ret_str += "/";
              ret_str += cor3;
              ret_str += "/";
              ret_str += cor4;
            }
          }
        } else {
          ret_str += "[ERROR] Deserializationに失敗。Reason :parse_flag_false";
        }
      }

    } else {
      Serial.printf("[HTTP] GET... FAILED, error: %s\n", http.errorToString(httpCode).c_str());
      ret_str = "[ERROR] HTTP GETに失敗。Reason :Connection failed";
    }

  } else {
    Serial.println(F("[HTTP] CAN'T begin HTTPclient!!"));
    ret_str = "[ERROR] HTTP GETに失敗。Reason :HTTPClient Error";
  }

  http.end();
  delay(100);
  Serial.println(F("[JSON] Returned!"));
  Serial.flush();

  return ret_str;
}
