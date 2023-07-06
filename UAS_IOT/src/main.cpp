#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <BH1750.h>
#include <Wire.h>
#include <Wifi.h>
#include <ThingsBoard.h>

#define SSID "Bbbbbbbb"
#define PASSWORD "aaaaaaaa"

#define DHTPIN 5
#define DHTTYPE DHT11
#define THINGSBOARD_SERVER "demo.thingsboard.io"
#define THINGSBOARD_ACCESS_TOKEN "YsAfrh0Ox14LLf4EpO9W"

DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter(0x23);
WiFiClient wifiClient;
ThingsBoard tb(wifiClient);

float temp, hum, light;
unsigned long sendDelay = 0;

void connect() {
  Serial.print("Connecting to ");
  Serial.print(SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(2000);
  Wire.begin();
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  connect(); 
}

void loop() {
  if (!tb.connected()) {
    if (tb.connect(THINGSBOARD_SERVER, THINGSBOARD_ACCESS_TOKEN)) {
      Serial.println("Connected to ThingsBoard");
    } else {
      Serial.println("Failed to connect");
      return;
    }
  }

  if (millis() - sendDelay > 2000) {
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    light = lightMeter.readLightLevel();
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println("°C");
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.println("RH");
    Serial.print("Light: ");
    Serial.print(light);
    Serial.println(" lux");
    tb.sendTelemetryFloat("temperature", temp);
    tb.sendTelemetryFloat("humidity", hum);
    tb.sendTelemetryFloat("light", light);
    sendDelay = millis();
  }

  tb.loop();
}

