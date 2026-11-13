// ESP8266: HTTPS needs the clock (the certificate's dates are checked), so set
// it before the first send(); or begin(..., false) to send over http as
// weather consoles do.

#include <ESP8266WiFi.h>
#include <NuaSense.h>
#include <time.h>

const char* WIFI_SSID = "your-wifi";
const char* WIFI_PASSWORD = "your-password";
const char* NUASENSE_TOKEN = "xxxx-xxxx-xxxx";

NuaSense nuasense;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (time(nullptr) < 1700000000) delay(500);  // until the clock is set
  nuasense.begin(NUASENSE_TOKEN, "esp8266-minimal");
}

void loop() {
  nuasense.add("temperature", 21.5);  // your sensor's reading here
  Serial.printf("NuaSense: %d\n", nuasense.send());
  delay(5 * 60 * 1000UL);
}
