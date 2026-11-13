#include "NuaSense.h"

#if defined(ESP32)
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#elif defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#else
#error "NuaSense supports ESP32 and ESP8266 boards"
#endif

// ISRG Root X2 (Let's Encrypt), valid until 2040-09-17: the root behind
// api.nuasense.com's certificate chain (checked 12.11.2026).
static const char ISRG_ROOT_X2[] PROGMEM = R"PEM(-----BEGIN CERTIFICATE-----
MIICGzCCAaGgAwIBAgIQQdKd0XLq7qeAwSxs6S+HUjAKBggqhkjOPQQDAzBPMQsw
CQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJuZXQgU2VjdXJpdHkgUmVzZWFyY2gg
R3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBYMjAeFw0yMDA5MDQwMDAwMDBaFw00
MDA5MTcxNjAwMDBaME8xCzAJBgNVBAYTAlVTMSkwJwYDVQQKEyBJbnRlcm5ldCBT
ZWN1cml0eSBSZXNlYXJjaCBHcm91cDEVMBMGA1UEAxMMSVNSRyBSb290IFgyMHYw
EAYHKoZIzj0CAQYFK4EEACIDYgAEzZvVn4CDCuwJSvMWSj5cz3es3mcFDR0HttwW
+1qLFNvicWDEukWVEYmO6gbf9yoWHKS5xcUy4APgHoIYOIvXRdgKam7mAHf7AlF9
ItgKbppbd9/w+kHsOdx1ymgHDB/qo0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0T
AQH/BAUwAwEB/zAdBgNVHQ4EFgQUfEKWrt5LSDv6kviejM9ti6lyN5UwCgYIKoZI
zj0EAwMDaAAwZQIwe3lORlCEwkSHRhtFcP9Ymd70/aTSVaYgLXTWNLxBo1BfASdW
tL4ndQavEi51mI38AjEAi/V3bNTIZargCyzuFJ0nN6T5U6VR5CmD1/iQMVtCnwr1
/q4AaOeMSQ+2b1tbFfLn
-----END CERTIFICATE-----
)PEM";

static String clean(const char* s, const char* allowed) {
  String out;
  for (const char* p = s; p && *p && out.length() < 48; ++p) {
    char c = *p;
    if (strchr(allowed, c) || isalnum(static_cast<unsigned char>(c))) out += c;
  }
  return out;
}

void NuaSense::begin(const char* token, const char* deviceId, bool https) {
  _token = clean(token, "");  // letters and digits; the dashes are optional
  _token.toLowerCase();
  _device = clean(deviceId, "._:-");
  _https = https;
  clear();
}

void NuaSense::clear() {
  _body = "";
  _count = 0;
}

bool NuaSense::add(const char* field, float value) {
  if (!field || !*field || isnan(value) || isinf(value) || _count >= 40) return false;
  for (const char* p = field; *p; ++p) {
    char c = *p;
    if (!(islower(static_cast<unsigned char>(c)) || isdigit(static_cast<unsigned char>(c)) || c == '_')) return false;
  }
  _body += ",\"";
  _body += field;
  _body += "\":";
  _body += String(value, 3);
  _count++;
  return true;
}

int NuaSense::send() {
  if (_token.length() != 12 && _token.length() != 24) return -10;  // not a push token
  if (_count == 0) return -11;                                      // nothing added
  if (WiFi.status() != WL_CONNECTED) return -12;

  String payload = "{\"device_id\":\"" + _device + "\",\"software\":\"nuasense-arduino/" NUASENSE_VERSION "\"" + _body + "}";
  String url = _https ? "https://api.nuasense.com/station/" : "http://ingest.nuasense.com/station/";
  url += _token;

  HTTPClient http;
  int status = -13;
#if defined(ESP32)
  WiFiClientSecure secure;
  WiFiClient plain;
  if (_https) {
    secure.setCACert(ISRG_ROOT_X2);
    if (!http.begin(secure, url)) return -13;
  } else if (!http.begin(plain, url)) {
    return -13;
  }
#else
  BearSSL::WiFiClientSecure secure;
  BearSSL::X509List anchor(ISRG_ROOT_X2);
  WiFiClient plain;
  if (_https) {
    secure.setTrustAnchors(&anchor);
    if (!http.begin(secure, url)) return -13;
  } else if (!http.begin(plain, url)) {
    return -13;
  }
#endif
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(10000);
  status = http.POST(payload);
  http.end();
  if (status == 200) clear();
  return status;
}
