#ifndef _SHT_H_
#define _SHT_H_
#include <Adafruit_SHT4x.h>

#define SHT_SCL 4
#define SHT_SDA 23
extern Adafruit_SHT4x sht4;

struct TempHumidity {
    float temperature;
    float humidity;
};
bool initSht();
TempHumidity getTempH();

#endif