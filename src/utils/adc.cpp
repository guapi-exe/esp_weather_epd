#include <driver/adc.h>
#include "esp_adc_cal.h"
#include "adc.h"
#include <Esp.h>

const float battery_min = 2.9;
const float battery_max = 3.9;
const float R1 = 100500.0;     
const float R2 = 100400.0;     
const float adc_offset = 0.00;
const float vol_offset = 0.00;
static esp_adc_cal_characteristics_t *adcChar;

uint32_t adc_read_val()
{
    int samplingFrequency = 500; 
    long sum = 0;             
    float samples = 0.0;      
    for (int i = 0; i < samplingFrequency; i++)
    {
        sum += adc1_get_raw(ADC_CHANNEL); 
        delayMicroseconds(1000);
    }
    samples = sum / (float)samplingFrequency;
    uint32_t voltage = esp_adc_cal_raw_to_voltage(samples, adcChar); 
    return voltage; 
}

void initAdc(){
    adc1_config_width(ADC_WIDTH_BIT);                     
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB); 

    adcChar = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t cal_mode = esp_adc_cal_characterize(ADC_NUM, ADC_ATTEN_DB, ADC_WIDTH_BIT, ESP_ADC_CAL_VAL_DEFAULT_VREF, adcChar);
    pinMode(ADC_EN, OUTPUT);
}

float batteryLevel(){
    digitalWrite(ADC_EN, 0);
    delay(100);
    float adc_voltage = (float)adc_read_val() / 1000.0f + adc_offset;
    float battery_voltage = (R1 + R2) / R2 * adc_voltage + vol_offset;
    float battery_level = ((battery_voltage - battery_min) / (battery_max - battery_min)) * 100;
    Serial.print("Battery Level: ");
    if (battery_level < 100)    Serial.print(battery_level);
    else                        Serial.print(100.0f); 
    return battery_level;
}
