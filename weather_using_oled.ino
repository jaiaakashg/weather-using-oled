#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const char* apiKey = "your api key";

const char* lat = "your latitude";  
const char* lon = "your longitude"; 
String weatherURL = "https://api.openweathermap.org/data/2.5/onecall?lat=" + String(lat) + "&lon=" + String(lon) + "&exclude=minutely,hourly&appid=" + String(apiKey) + "&units=metric";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(1000); 

  Serial.println();
  Serial.println("Starting...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  fetchWeatherData();
}

void fetchWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    client->setInsecure();

    HTTPClient https;
    https.begin(*client, weatherURL);
    int httpCode = https.GET();

    if (httpCode > 0) {
      String payload = https.getString();
      Serial.println(payload); 

      StaticJsonDocument<4096> doc; 
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }

      float currentTemp = doc["current"]["temp"];
      float windSpeed = doc["current"]["wind_speed"];
      int clouds = doc["current"]["clouds"];
      float pressure = doc["current"]["pressure"];

   
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);

      display.setCursor(0, 0);
      display.print("Chennai Weather");

      display.setCursor(0, 10);
      display.print("Temp: ");
      display.print(currentTemp);
      display.println(" °C");

      display.setCursor(0, 20);
      display.print("Wind: ");
      display.print(windSpeed);
      display.println(" m/s");

      display.setCursor(0, 30);
      display.print("Clouds: ");
      display.print(clouds);
      display.println(" %");

      display.setCursor(0, 40);
      display.print("Pressure: ");
      display.print(pressure);
      display.println(" hPa");

      display.display();
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpCode);
    }
    https.end();
  }
}

void loop() {
}

