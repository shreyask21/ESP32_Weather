#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "weather_bmp.h"
#include "bitmaps.h"
#include <fonts/FreeSerifBold12pt7b.h>
#include <fonts/FreeSansBold9pt7b.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#define TFT_CS         15
#define TFT_RST        4
#define TFT_DC         2

WiFiMulti WiFiMulti;
Adafruit_ST7789      tft    = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

const char * ssid = "WIFI SSID HERE";
const char * psw = "WIFI PASSWORD HERE";
String city =  "pune, in"; //Enter your city name:  E.g. "pune,in"
String key = "API KEY HERE";

void setup() {
  tft.init(240, 240);
  tft.setRotation(2);
  tft.fillScreen(0);
  ConnectToWifi();
}

void loop() {
  getCurrentWeather();
  delay(30000);
}

void getCurrentWeather() {
  char* temperature_deg;
  String current = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=metric" + "&APPID=" + key;
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(current);
    if (http.GET() > 0) {
      const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 600;
      DynamicJsonDocument doc(capacity);
      DeserializationError error = deserializeJson(doc, http.getString());
      int id = doc["weather"][0]["id"];
      String type = doc["weather"][0]["main"];
      float temperature = doc["main"]["temp"];
      int humidity = doc["main"]["humidity"];
      int pressure = doc["main"]["pressure"];
      String description = doc["weather"][0]["description"];
      float wind_speed = doc["wind"]["speed"];
      int wind_deg = doc["wind"]["deg"];
      int rain_volume = doc["rain"]["3h"];
      int cloud = doc["clouds"]["all"];
      int bitmapid;
      bool isDay = ((intTime(doc["dt"]) >= intTime(doc["sys"]["sunrise"])) && (intTime(doc["dt"]) <= intTime(doc["sys"]["sunset"])));
      String direction;
      if (wind_deg > 22.5) direction = "NE";
      if (wind_deg > 67.5) direction =  "E";
      if (wind_deg > 122.5) direction =  "SE";
      if (wind_deg > 157.5) direction =  "S";
      if (wind_deg > 202.5) direction =  "SW";
      if (wind_deg > 247.5) direction =  "W";
      if (wind_deg > 292.5) direction =  "NW";
      if (wind_deg > 337.5) direction =  "N";
      delay(5000);
      tft.fillScreen(0);
      if (id < 233) tft.drawBitmap(70, 2, thunder, 100, 100, ST77XX_WHITE);
      if (id > 299 && id < 322) tft.drawBitmap(70, 0, sleet, 100, 100, ST77XX_WHITE);
      if (id > 499 && id < 502) tft.drawBitmap(70, 0, shower, 100, 100, ST77XX_WHITE);
      if (id > 501 && id < 532) tft.drawBitmap(70, 0, rain, 100, 100, ST77XX_WHITE);
      if (id > 599 && id < 623) tft.drawBitmap(70, 0, snow, 100, 100, ST77XX_WHITE);
      if (id > 699 && id < 782) tft.drawBitmap(70, 0, fog, 100, 100, ST77XX_WHITE);
      if (id == 800 && isDay) tft.drawBitmap(70, 0, sunny, 100, 100, ST77XX_WHITE);
      if (id == 800 && !isDay) tft.drawBitmap(70, 0, stars, 100, 100, ST77XX_WHITE);
      if (id > 800 && id < 803) tft.drawBitmap(70, 0, cloudy, 100, 100, ST77XX_WHITE);
      if (id > 802) tft.drawBitmap(70, 0, clouds, 100, 100, ST77XX_WHITE);
      tft.setCursor(0, 110);
      tft.setFont(&FreeSansBold9pt7b);
      //tft.println(description);
      tft.print("Temperature       : ");
      tft.print(temperature);
      tft.println("C");

      tft.print("Wind                    : ");
      tft.println(direction);

      tft.print("Wind Speed        : ");
      tft.print(wind_speed, 1);
      tft.println("m/s");

      tft.print("Humidity             : ");
      tft.print(humidity);
      tft.println("%");

      tft.print("Pressure             : ");
      tft.print(pressure);
      tft.println("hPa");

      tft.print("Cloud Coverage : ");
      tft.print(cloud);
      tft.println("%");

      http.end();
    }
  }
}
void ConnectToWifi() {
  tft.setCursor(0, 135);
  tft.setFont(&FreeSerifBold12pt7b);
  tft.setTextColor(ST77XX_RED);
  tft.drawBitmap(56, 2, wifi1, 128, 128, ST77XX_WHITE);
  tft.println("Connecting to:");
  tft.println(ssid);
  WiFiMulti.addAP(ssid, psw);
  while (WiFiMulti.run() != WL_CONNECTED) {
  }
  tft.fillScreen(0);
  tft.drawBitmap(56, 2, wifi2, 128, 128, ST77XX_WHITE);
  tft.setCursor(0, 135);
  tft.setTextColor(ST77XX_GREEN);
  tft.println("WiFi Connected");
  tft.println("IP:");
  tft.println(WiFi.localIP());
  tft.print("Signal: ");
  int sig = WiFi.RSSI();
  if ((-1 * sig) <= 70) tft.setTextColor(ST77XX_CYAN);
  if ((-1 * sig) > 70 && (-1 * sig) < 80) tft.setTextColor(ST77XX_GREEN);
  if ((-1 * sig) > 80 && (-1 * sig) < 85) tft.setTextColor(ST77XX_YELLOW);
  if ((-1 * sig) > 85) tft.setTextColor(ST77XX_RED);
  tft.print(sig);
  tft.print("dBm");
  delay(4000);
  tft.setTextColor(ST77XX_WHITE);
  tft.fillScreen(0);
  tft.drawBitmap(70, 0, question, 100, 100, ST77XX_WHITE);
  tft.setCursor(0, 135);
  tft.setFont(&FreeSerifBold12pt7b);
  tft.println("Connecting To \nOpenweathermap...");
  tft.println("Getting Weather Data");
}

int intTime(String timestamp) {
  long int time_int = timestamp.toInt();
  struct tm *info;
  time_t t = time_int;
  info = gmtime(&t);
  int result = info->tm_hour * 100 + info->tm_min;
  return result;
}
