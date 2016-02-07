#include <ESP8266WiFi.h>
#include "DHT.h"

// run locally, ie. don't push values to server. values are logged to serial
#define RUN_LOCAL 1

// DHT11 sensor. Connect:
//   pin 1 (vcc) to 3v3
//   pin 2 (data) to pin defined DHTPIN below
//   pin 4 (ground) to GND
#define DHTPIN D6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const boolean useFahrenheit = false;

// WIFI connection details
const char* WIFI_SSID     = "<wifi ssid>";
const char* WIFI_PASSWORD = "<wifi password>";

// push to local test server for now (run test/httpserver.js)
const char* CLOUD_HOST = "192.168.1.102";
const int   CLOUD_PORT = 8888;

const short READ_FREQUENCY = 10000;

// PM2.5 read (random!)
const byte PM25_RAND_RANGE = 100;
const byte PM25_RAND_FLOOR = 22;
long readPM25() {
  return random(PM25_RAND_RANGE) + PM25_RAND_FLOOR;
}

float readTemperature() {
  return dht.readTemperature(useFahrenheit);
}

float readHumidity() {
  return dht.readHumidity();
}

float readHeatIndex(float temperature, float humidity) {
  return dht.computeHeatIndex(temperature, humidity, useFahrenheit);
}

void setup() {
  dht.begin();
  randomSeed(analogRead(0));
  Serial.begin(115200);

  if (!RUN_LOCAL) {
    delay(10);
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  delay(READ_FREQUENCY);

  WiFiClient client;
  if (!RUN_LOCAL) {
    if (!client.connect(CLOUD_HOST, CLOUD_PORT)) {
      Serial.println("connection failed");
      return;
    }
  }

  // DHT11 reads
  float temperature = readTemperature();
  float humidity = readHumidity();
  float heatIndex = readHeatIndex(temperature, humidity);
  if (isnan(temperature) || isnan(humidity) || isnan(heatIndex)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // PM25 reads
  long pm25 = readPM25();

  // push reads to server
  String url = "/reading?";
  url += "temp=";
  url += temperature;
  url += "&&humidity=";
  url += humidity;
  url += "&&heatIndex=";
  url += heatIndex;
  url += "&&pm25=";
  url += pm25;
  Serial.println("URL:" + url);
  if (!RUN_LOCAL) {
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + CLOUD_HOST + "\r\n" + 
                 "Connection: close\r\n\r\n");
    delay(10);

    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }

}

