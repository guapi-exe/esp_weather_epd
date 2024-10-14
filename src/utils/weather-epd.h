#ifndef WEATHER_EPD_H
#define WEATHER_EPD_H
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "weather.h"

extern GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
void weather_epd(const Weather* weather);
void insights_epd(const Weather* weather);
void test_epd();
void insights_epd2(const Weather* weather);

#endif
