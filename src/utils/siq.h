#ifndef _SIQ_H_
#define _SIQ_H_

#define PIN_A 33
#define PIN_B 14
#define PIN_S 22
extern volatile int encoderValue;
extern volatile bool buttonPressed;
void IRAM_ATTR handleEncoder();
void IRAM_ATTR handleButton();
void siqInit();

#endif