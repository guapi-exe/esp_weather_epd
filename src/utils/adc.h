#ifndef _ADC_H_
#define _ADC_H_

#include "esp_adc_cal.h"
#define ADC_CHANNEL     ADC1_CHANNEL_6
#define ADC_ATTEN_DB    ADC_ATTEN_DB_6
#define ADC_WIDTH_BIT   ADC_WIDTH_BIT_12
#define ADC_NUM         ADC_UNIT_1    
#define ADC_EN          32
void initAdc();
float batteryLevel();

#endif