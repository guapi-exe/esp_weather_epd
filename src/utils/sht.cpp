#include <Adafruit_SHT4x.h>
#include <Wire.h>
#include "sht.h"


bool initSht() {
    sht4 = Adafruit_SHT4x();
    Wire.begin(SHT_SCL, SHT_SDA);
    if (!sht4.begin(&Wire)) {
        Serial.println("Couldn't find SHT4x");
        return false;
    } else {
        return true;
    }
}

TempHumidity getTempH() {
    sensors_event_t humidity, temp;
    sht4.getEvent(&humidity, &temp);
    TempHumidity result;
    result.temperature = temp.temperature;
    result.humidity = humidity.relative_humidity;
    return result;
}
