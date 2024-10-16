#ifndef SETTING_EPD_H
#define SETTING_EPD_H
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "weather.h"

extern GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
void setting_epd();
void pointer_epd(int linex);
void times_epd(int time);
void start_epd();
void startText_epd(String text);

#endif