#include <Arduino.h>
#include "siq.h"

volatile int encoderValue = 0;
volatile bool buttonPressed = false;
volatile int lastStateA = 0;

void IRAM_ATTR handleEncoder() {
    int stateA = digitalRead(PIN_A);
    if (stateA != lastStateA) { // 状态发生变化
        if (digitalRead(PIN_B) != stateA) {
            encoderValue++;
        } else {
            encoderValue--;
        }
    }
    lastStateA = stateA;
}

void IRAM_ATTR handleButton() {
    buttonPressed = true;
}

void siqInit() {
    pinMode(PIN_A, INPUT_PULLUP);
    pinMode(PIN_B, INPUT_PULLUP);
    pinMode(PIN_S, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(PIN_A), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_S), handleButton, FALLING);
}
