#ifndef _WIFI_H_
#define _WIFI_H_
#include <Preferences.h>
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>

#define WIFI_SSID "guapi"
#define WIFI_PASSWORD "123456"
extern Preferences preferences; 
extern GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
bool connectToWiFi();
void saveWiFiConfig(const char* ssid, const char* password);

#endif