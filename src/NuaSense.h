// NuaSense for Arduino (ESP32, ESP8266): send your own sensor's readings to
// NuaSense and get the station check, frost and spray windows, irrigation and
// disease risk for your station.
//
//   NuaSense ns;
//   ns.begin("kw7n-4hpd-x2vt", "garden-station");   // token from the NuaSense console
//   ns.add("temperature", 21.4);                    // °C
//   ns.add("soil_moisture_2", 34);                  // %, second soil probe
//   int status = ns.send();                         // 200 = stored
//
// Field names and units: https://github.com/nuasense/station-schema
#pragma once

#include <Arduino.h>

#define NUASENSE_VERSION "0.1.1"

class NuaSense {
 public:
  // token: the station's push token from the console (External stations →
  //   Add a station → "I built it myself"); dashes are optional.
  // deviceId: any name for this device; it binds the token to it.
  // https: true sends to https://api.nuasense.com (certificate checked against
  //   ISRG Root X2). The ESP8266 then needs the clock set (configTime) first;
  //   false sends to http://ingest.nuasense.com, as weather consoles do.
  void begin(const char* token, const char* deviceId, bool https = true);

  // Adds one value. Names: [a-z0-9_] only (temperature, soil_moisture_2, …);
  // NaN and bad names are skipped. Returns false when skipped.
  bool add(const char* field, float value);

  // Posts what was added and clears it. The HTTP status (200 = stored, 404 =
  // unknown token, 409 = the token belongs to another device, 429 = too often),
  // or a negative number when the connection failed. At most one upload a
  // minute is kept; uploads in between are answered 200 and dropped.
  int send();

  void clear();
  uint8_t count() const { return _count; }

 private:
  String _token;
  String _device;
  bool _https = true;
  String _body;
  uint8_t _count = 0;
};
