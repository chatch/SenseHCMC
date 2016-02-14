/*
 * Read values from a DHT11 sensor and push them to a private Blynk server via WIFI.
 */
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include "DHT.h"

// IMPORTANT: set auth properties in here first
#include "AuthProperties.h" 

// DHT11 sensor. Connect:
//   pin 1 (vcc) to 3v3
//   pin 2 (data) to pin defined DHTPIN below
//   pin 4 (ground) to GND
#define DHTPIN D6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Blynk
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#define PIN_TEMPERATURE V1
#define PIN_HUMIDITY V2

const boolean RUN_LOCAL = false;
const boolean USE_FAHRENHEIT = false;
const short PUSH_INTERVAL = 10000;

SimpleTimer timer;

float readTemperature() {
  return dht.readTemperature(USE_FAHRENHEIT);
}

float readHumidity() {
  return dht.readHumidity();
}

float readHeatIndex(float temperature, float humidity) {
  return dht.computeHeatIndex(temperature, humidity, USE_FAHRENHEIT);
}

void sendDHTValue(float value, int pin, String label)
{
  if (isnan(value)) {
    Serial.println("Failed to read " +  label);
    return;
  }
  Serial.print(label + ": ");
  Serial.println(value);
  if (!RUN_LOCAL)
    Blynk.virtualWrite(pin, value);
}

void sendTemperature()
{
  float temperature = readTemperature();
  sendDHTValue(temperature, PIN_TEMPERATURE, "temperature");
}

void sendHumidity()
{
  float humidity = readHumidity();
  sendDHTValue(humidity, PIN_HUMIDITY, "humidity");
}

void setup() {
  dht.begin();
  Serial.begin(9600);
  if (!RUN_LOCAL)
    Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASSWORD, BLYNK_HOST);
  timer.setInterval(PUSH_INTERVAL, sendTemperature);
  timer.setInterval(PUSH_INTERVAL, sendHumidity);
}

void loop() {
  Blynk.run();
  timer.run();
}

