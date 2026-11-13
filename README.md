# NuaSense for Arduino

Send your own sensors' readings to [NuaSense](https://nuasense.com) from an ESP32 or ESP8266:
weather, soil, water, greenhouse, beehive, storage. In return, NuaSense checks every reading
(is your sensor right, and if not, what to fix), and computes frost, spray windows, irrigation
and disease risk for your station.

```cpp
#include <NuaSense.h>

NuaSense nuasense;

void setup() {
  // WiFi first, then:
  nuasense.begin("xxxx-xxxx-xxxx", "garden-station");  // token from the NuaSense console
}

void loop() {
  nuasense.add("temperature", 21.4);      // °C
  nuasense.add("humidity", 63);           // %
  nuasense.add("soil_moisture_2", 34);    // %, second soil probe
  int status = nuasense.send();           // 200 = stored
  delay(5 * 60 * 1000UL);
}
```

## Get a token

1. Sign in at [platform.nuasense.com](https://platform.nuasense.com) (free).
2. **External stations → Add a station → I built it myself**, set the station's place on the map.
3. Copy the token. Your first three stations are free.

## Names and units

One `add()` per value, with NuaSense's names and units: `temperature` (°C), `humidity` (%),
`pressure` (hPa at the station), `wind_speed` (m/s), `rain_mm` (mm since the last upload),
`soil_moisture` (%), `soil_temperature` (°C), `soil_ec` (mS/cm), `water_level` (m), `co2` (ppm),
`hive_weight` (kg), `storage_temperature` (°C) and more. Several sensors of one kind:
`soil_moisture_2` … `soil_moisture_8`. The full list:
[nuasense-reading.schema.json](https://github.com/nuasense/station-schema/blob/main/nuasense-reading.schema.json).
A name that is not on the list is not stored.

## What send() returns

| Value | Meaning |
|---|---|
| 200 | stored (uploads less than a minute apart are answered 200 and dropped) |
| 404 | unknown token |
| 409 | the token belongs to another device (`deviceId`) |
| 429 | too many uploads |
| < 0 | no connection: −10 bad token, −11 nothing added, −12 no WiFi, other negatives from the HTTP client |

## HTTPS

`begin(token, deviceId)` sends to `https://api.nuasense.com` and checks the certificate against
ISRG Root X2 (Let's Encrypt, valid until 2040). On an ESP8266, set the clock first
(`configTime`, see `extras/ESP8266_Minimal`). `begin(token, deviceId, false)` sends over plain
http to `ingest.nuasense.com`, as weather consoles do.

## Examples

- `ESP32_BME280`: a weather station from an ESP32 and a BME280.
- `ESP32_SoilMoisture`: two capacitive soil moisture sensors.

MIT licence.
