// A weather station from an ESP32 and a BME280 (temperature, humidity,
// pressure), sending to NuaSense every five minutes.
//
// Wiring: BME280 SDA → GPIO21, SCL → GPIO22, VIN → 3V3, GND → GND.
// Mount the sensor in the shade, 1.5–2 m over grass, in a radiation shield:
// NuaSense checks every reading and tells you when the sun reaches it.
//
// Libraries: NuaSense, Adafruit BME280 Library (with Adafruit Unified Sensor).

#include <Adafruit_BME280.h>
#include <NuaSense.h>
#include <WiFi.h>

const char* WIFI_SSID = "your-wifi";
const char* WIFI_PASSWORD = "your-password";
// From the NuaSense console: External stations → Add a station → "I built it myself".
const char* NUASENSE_TOKEN = "xxxx-xxxx-xxxx";

Adafruit_BME280 bme;
NuaSense nuasense;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  if (!bme.begin(0x76)) Serial.println("BME280 not found at 0x76 (try 0x77)");
  nuasense.begin(NUASENSE_TOKEN, "esp32-bme280");
}

void loop() {
  nuasense.add("temperature", bme.readTemperature());          // °C
  nuasense.add("humidity", bme.readHumidity());                // %
  nuasense.add("pressure", bme.readPressure() / 100.0F);       // hPa at the station
  int status = nuasense.send();
  Serial.printf("NuaSense: %d\n", status);                    // 200 = stored
  delay(5 * 60 * 1000UL);
}
