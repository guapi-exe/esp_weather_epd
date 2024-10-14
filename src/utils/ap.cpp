#include "ap.h"
#include <WiFi.h>


void openAP(){
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
}

void closeAP(){
    WiFi.softAPdisconnect(true);
    Serial.print("AP Close");
}

