#include "wifi.h"
#include <WiFi.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Preferences.h>

struct WiFiConfig {
    String ssid;
    String password;
};

void saveWiFiConfig(const char* ssid, const char* password) {
    preferences.begin("wifi-config", false); 
    preferences.putString("ssid", ssid);     
    preferences.putString("password", password); 
    preferences.end(); 
    Serial.println("Wi-Fi credentials saved.");
}


WiFiConfig readWiFiConfig() {
    preferences.begin("wifi-config", true); 
    WiFiConfig config;
    config.ssid = preferences.getString("ssid", "");
    config.password = preferences.getString("password", "");
    preferences.end(); 
    return config; 
}

bool connectToWiFi() {
    WiFiConfig config = readWiFiConfig();
    String ssid = config.ssid;
    String password = config.password;
    display.setRotation(0); // 设置屏幕旋转
    display.setFullWindow();
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    if (ssid != "" && password != "") {
        WiFi.begin(ssid.c_str(), password.c_str());
        Serial.print("Connecting to Wi-Fi: ");
        Serial.println(ssid);
        u8g2Fonts.setCursor(12, 25);
        u8g2Fonts.print("网络连接中:"+ssid);
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20) {
            delay(500);
            Serial.print(".");
            retries++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nConnected to Wi-Fi");
            u8g2Fonts.setCursor(12, 55);
            u8g2Fonts.print("网络连接成功");
            delay(1000);
            return true;  
        } else {
            Serial.println("\nFailed to connect to Wi-Fi");
            return false;  
        }
    } else if(WIFI_SSID != "" && WIFI_PASSWORD != ""){
        saveWiFiConfig(WIFI_SSID,WIFI_PASSWORD);
        connectToWiFi();
    } else {
        u8g2Fonts.setCursor(12, 55);
        u8g2Fonts.print("网络连接失败");
        return false;
    }
    return false; 
}