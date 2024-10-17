#define USE_HSPI_FOR_EPD
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Adafruit_SHT4x.h>
#include "utils/sht.h"
#include "utils/weather-epd.h"
#include "utils/deta-epd.h"
#include "utils/wifi.h"
#include "utils/weather.h"
#include "utils/ap.h"
#include "utils/webserver.h"
#include "utils/siq.h"
#include "utils/adc.h"
#include "utils/setting-epd.h"
#include <Preferences.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include <esp_sleep.h>

#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_BW_IS_GxEPD2_BW true

#define IS_GxEPD(c, x) (c##x)
#define IS_GxEPD2_BW(x) IS_GxEPD(GxEPD2_BW_IS_, x)

#if defined(ESP32)
#define MAX_DISPLAY_BUFFER_SIZE 65536ul
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#endif
GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display(GxEPD2_420_M01(5, 17, 16, 13));
#endif

#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
SPIClass hspi(HSPI);
#endif

RTC_DATA_ATTR int displayIndex = 2; 
RTC_DATA_ATTR int lastEncoderValue = 0;
RTC_DATA_ATTR int updateTime = 0;
RTC_DATA_ATTR unsigned long lastWeatherUpdate = 0; 
RTC_DATA_ATTR unsigned long wakeTime = 0;
RTC_DATA_ATTR int mode = 0; //0:正常 1:无网络连接 2:未配置api 
RTC_DATA_ATTR int linex = -1; 
RTC_DATA_ATTR int wakeupReason = 0; // 0: 未知, 1: 波轮, 2: 定时
bool setting = false;
unsigned long lastTimesUpdate = 0;

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
Weather weather;
Preferences preferences;
DNSServer dnsServer;
Adafruit_SHT4x sht4;

const unsigned long SLEEP_DURATION = 600 * 1000 * 1000; // 10分钟轮换ui
const unsigned long WAKE_DURATION = 10 * 1000; // 10秒睡眠醒来后的延迟
const unsigned long AP_UPDATE_INTERVAL = 5 * 60 * 1000; // 5分钟 AP模式下温湿度更新延迟

void updateDisplay() {
    switch (displayIndex) {
        case 0:
            weather_epd(&weather);
            break;
        case 1:
            insights_epd(&weather);
            break;
        case 2:
            insights_epd2(&weather);
            break;
        case 3:
            deta_epd(&weather);
            break;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.print("Times before setup: ");
    Serial.println(updateTime);
    esp_sleep_wakeup_cause_t wakeupCause = esp_sleep_get_wakeup_cause();
    switch (wakeupCause) {
        case ESP_SLEEP_WAKEUP_EXT0:
            wakeupReason = 1; // 波轮唤醒
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            wakeupReason = 2; // 定时唤醒
            break;
        default:
            wakeupReason = 0; // 未知原因
            break;
    }
    Serial.println(wakeupCause);
    Serial.println(wakeupReason);
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS挂载失败");
        return;
    }

    #if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
        hspi.begin(18, 12, 19, 15); 
        display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
    #endif

    display.init(115200);
    initSht();
    initAdc();
    siqInit();

    //
    
    if (wakeupReason == 1) {
        setting_epd(); // 波轮强制唤醒进入设置模式
        pointer_epd(0);
        setting = true;
    } else if (updateTime >= 6 || updateTime == 0) {
        start_epd();
        startText_epd("网络连接中...");
        if (connectToWiFi()) {
            mode = 0;
        } else {
            mode = 1; // mode 1无网络连接 
        }
        delay(100);
        startText_epd("API数据获取中...");
        weather = getWeatherAll();
        updateTime = 1;
        writeWeather(&weather);
        startText_epd("启动中...");
        delay(100);
        insights_epd2(&weather);
    } else {
        updateTime++;
        displayIndex = (displayIndex + 1) % 4;
        delay(100);
        weather = readWeather();
        if (mode == 1 || mode == 2) {
            displayIndex = 2;
        }
        updateDisplay();
    }

    if (mode == 1 || mode == 2) {
        openAP();
        initWebServer();
    }

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0); 
    wakeTime = millis();

     
    
}

void loop() {
    
    unsigned long currentTime = millis();
    
    // AP模式下进入设置界面
    if (currentTime - wakeTime >= WAKE_DURATION && (mode == 1 || mode == 2) && !setting) {
        if (encoderValue != lastEncoderValue) {
            setting_epd(); 
            pointer_epd(0);
            setting = true;
        }
    }
    
    // 编码器值变化处理
    if (encoderValue != lastEncoderValue) {
        Serial.print("编码器值: ");
        Serial.println(encoderValue);
        linex = (linex + 1) % 6;
        if(setting){
            pointer_epd(linex);
        }
    
        displayIndex = (displayIndex + 1) % 4;
        if (mode == 1  || mode == 2) {
            displayIndex = 2;
        }
        lastEncoderValue = encoderValue;
        if(!setting){
            updateDisplay();
        }
        currentTime = millis();
        wakeTime = millis();
    }
    
    Serial.println(currentTime - wakeTime);
    
    // 每2秒更新一次times_epd
    if (currentTime - lastTimesUpdate >= 2000 && setting) {
        times_epd((currentTime - wakeTime) / 1000);
        lastTimesUpdate = currentTime;
    }
    
    // 处理WAKE_DURATION超时
    if (currentTime - wakeTime >= WAKE_DURATION) {
        if (setting) {
            weather = readWeather();
            switch (linex) {
                case 0:
                    weather_epd(&weather);
                    setting = false;
                    break;
                case 1:
                    insights_epd(&weather);
                    setting = false;
                    break;
                case 2:
                    insights_epd2(&weather);
                    setting = false;
                    break;
                case 3:
                    deta_epd(&weather);
                    setting = false;
                    break;
                case 4:
                    mode = 1;
                    openAP();
                    initWebServer();
                    insights_epd2(&weather);
                    setting = false;
                    break;
                case 5:
                    ESP.restart();
                    break;
            }
        }
        
        if (mode == 0) { // 非AP模式下进入睡眠状态
            esp_sleep_enable_timer_wakeup(SLEEP_DURATION);
            esp_deep_sleep_start();
        }
    }
    
    // AP模式下更新温湿度
    if (currentTime - wakeTime >= AP_UPDATE_INTERVAL && (mode == 1 || mode == 2)) {
        updateSHT();
    }
    
    delay(100);
}
