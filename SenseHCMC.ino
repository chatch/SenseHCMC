#include <ESP8266WiFi.h>

// WIFI connection details
const char* WIFI_SSID     = "<wifi ssid>";
const char* WIFI_PASSWORD = "<wifi password>";

// push to local test server for now (run test/httpserver.js)
const char* CLOUD_HOST = "192.168.1.102";
const int   CLOUD_PORT = 8888;

const short READ_FREQUENCY = 5000;

// PM2.5 read
const byte PM25_RAND_RANGE = 100;
const byte PM25_RAND_FLOOR = 22;
long getSensorReadPM25() {
  return random(PM25_RAND_RANGE) + PM25_RAND_FLOOR;
}

// Temperature read
const byte TEMP_RAND_RANGE = 10;
const float TEMP_RAND_FLOOR = 24.0;
float getSensorReadTemperature() {
  return TEMP_RAND_FLOOR + random(TEMP_RAND_RANGE);
}

void setup() {
  randomSeed(analogRead(0));

  Serial.begin(115200);
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

void loop() {
  delay(READ_FREQUENCY);

  WiFiClient client;
  if (!client.connect(CLOUD_HOST, CLOUD_PORT)) {
    Serial.println("connection failed");
    return;
  }

  // get sensor reads
  long readPM25 = getSensorReadPM25();
  float readTemperature = getSensorReadTemperature();
  Serial.print("PM2.5 = ");
  Serial.print(readPM25);
  Serial.print("; temp = ");
  Serial.println(readTemperature);

  // push reads to server
  String url = "/reading?pm25=";
  url += readPM25;
  url += "temp=";
  url += readTemperature;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + CLOUD_HOST + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

}

