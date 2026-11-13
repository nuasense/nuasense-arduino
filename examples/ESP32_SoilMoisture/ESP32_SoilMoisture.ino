// Two capacitive soil moisture sensors on an ESP32, sent to NuaSense as
// soil_moisture (bed 1) and soil_moisture_2 (bed 2).
//
// Calibrate each sensor once: note the raw value in dry air (DRY) and in a
// glass of water (WET). The percentage between them is relative moisture, not
// a laboratory value; NuaSense uses its change over time for watering.

#include <NuaSense.h>
#include <WiFi.h>

const char* WIFI_SSID = "your-wifi";
const char* WIFI_PASSWORD = "your-password";
const char* NUASENSE_TOKEN = "xxxx-xxxx-xxxx";

const int PINS[] = {34, 35};
const int DRY[] = {2900, 2900};
const int WET[] = {1200, 1200};

NuaSense nuasense;

float percent(int raw, int dry, int wet) {
  float p = 100.0F * (dry - raw) / float(dry - wet);
  return p < 0 ? 0 : (p > 100 ? 100 : p);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  nuasense.begin(NUASENSE_TOKEN, "esp32-soil");
}

void loop() {
  nuasense.add("soil_moisture", percent(analogRead(PINS[0]), DRY[0], WET[0]));
  nuasense.add("soil_moisture_2", percent(analogRead(PINS[1]), DRY[1], WET[1]));
  Serial.printf("NuaSense: %d\n", nuasense.send());
  delay(10 * 60 * 1000UL);
}
