#include "weather-epd.h"
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "Bitmap.cpp"
#include "weather.h"
#include "adc.h"
#include <WiFi.h>
#include <vector>
#include <map>
#include <string>
#include <locale>
#include <codecvt>
#include "main.h"

std::vector<std::wstring> splitStringByWidth(U8G2_FOR_ADAFRUIT_GFX &u8g2, const std::wstring &str, int maxWidth) {
    std::vector<std::wstring> result;
    std::wstring currentLine;
    int currentWidth = 0;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    for (const auto &ch : str) {
        std::string utf8Char = conv.to_bytes(ch);
        int charWidth = u8g2.getUTF8Width(utf8Char.c_str());

        if (currentWidth + charWidth > maxWidth) {
            result.push_back(currentLine);
            currentLine.clear();
            currentWidth = 0;
        }

        currentLine += ch;
        currentWidth += charWidth;
    }

    if (!currentLine.empty()) {
        result.push_back(currentLine);
    }

    return result;
}

void drawTextWithWrap(U8G2_FOR_ADAFRUIT_GFX &u8g2, int x, int y, int width, int lines, const std::wstring &text) {
    int cursorX = x;
    int cursorY = y;
    int lineHeight = 12; 
    std::vector<std::wstring> splitText = splitStringByWidth(u8g2, text, width);

    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    int lineCount = 0; 

    for (const auto &s : splitText) {
        if (lineCount >= lines) break;

        //Serial.println(conv.to_bytes(s).c_str());
        u8g2.setCursor(cursorX, cursorY);
        u8g2.print(conv.to_bytes(s).c_str());
        cursorY += lineHeight;
        lineCount++; 
    }
}


void drawRoundedTriangle(int centerX, int centerY, int radius, float angleDegrees, int cornerRadius) {
  float angleRadians = radians(angleDegrees);

  int x1 = centerX;
  int y1 = centerY - radius;
  int x2 = centerX - radius * 0.5;
  int y2 = centerY + radius * 0.5;
  int x3 = centerX + radius * 0.5;
  int y3 = centerY + radius * 0.5;

  int xr1 = centerX + (x1 - centerX) * cos(angleRadians) - (y1 - centerY) * sin(angleRadians);
  int yr1 = centerY + (x1 - centerX) * sin(angleRadians) + (y1 - centerY) * cos(angleRadians);
  int xr2 = centerX + (x2 - centerX) * cos(angleRadians) - (y2 - centerY) * sin(angleRadians);
  int yr2 = centerY + (x2 - centerX) * sin(angleRadians) + (y2 - centerY) * cos(angleRadians);
  int xr3 = centerX + (x3 - centerX) * cos(angleRadians) - (y3 - centerY) * sin(angleRadians);
  int yr3 = centerY + (x3 - centerX) * sin(angleRadians) + (y3 - centerY) * cos(angleRadians);

  display.fillTriangle(xr1, yr1, xr2, yr2, xr3, yr3, GxEPD_BLACK);

}

void drawHumidity(U8G2_FOR_ADAFRUIT_GFX &u8g2, int percentage, int centerX, int centerY, int radius) {
    int startY = centerY - radius / 2;
    int endY = centerY + radius / 2;
    
    for (int angle = 0; angle <= radius; angle++) {
        int y = startY + angle;
        display.fillCircle(centerX + 5, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX + 15, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX + 25, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX - 5, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX - 15, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX - 25, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX - 35, y, 4, GxEPD_BLACK);
    }
    
    for (int angle = 0; angle <= radius; angle++) {
        int y = startY + angle;
        display.fillCircle(centerX + 5, y, 3, GxEPD_WHITE);
        display.fillCircle(centerX + 15, y, 3, GxEPD_WHITE);
        display.fillCircle(centerX + 25, y, 3, GxEPD_WHITE);
        display.fillCircle(centerX - 5, y, 3, GxEPD_WHITE);
        display.fillCircle(centerX - 15, y, 3, GxEPD_WHITE);
        display.fillCircle(centerX - 25, y, 3, GxEPD_WHITE);
        display.fillCircle(centerX - 35, y, 3, GxEPD_WHITE);
    }

    for (int angle = 0; angle <= (int)((radius / 100.0) * percentage); angle++) {
        int y = endY - angle;
        display.fillCircle(centerX + 5, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX + 15, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX + 25, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX - 5, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX - 15, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX - 25, y, 4, GxEPD_BLACK);
        display.fillCircle(centerX - 35, y, 4, GxEPD_BLACK);
    }
}


void drawGauge(U8G2_FOR_ADAFRUIT_GFX &u8g2, int percentage, int centerX, int centerY, int radius,const String text) {
    int startAngle = 135;
    int endAngle = 135;
    int angleRange = 270;

    for (int angle = startAngle; angle <= endAngle + angleRange; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 4, GxEPD_BLACK);
    }
    for (int angle = startAngle; angle <= endAngle + angleRange; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 3, GxEPD_WHITE);
    }

    int progressAngle = startAngle + (percentage * angleRange / 100);
    for (int angle = startAngle; angle <= progressAngle; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 4, GxEPD_BLACK);
        if(progressAngle == angle){display.fillCircle(x, y, 5, GxEPD_BLACK);}
    }
        
    u8g2.setCursor(centerX-12, centerY+12);
    u8g2.print(text);
}

void drawPrecipitation(U8G2_FOR_ADAFRUIT_GFX &u8g2, int centerX, int centerY, int radius,const String text){
    display.drawCircle(centerX, centerY, radius, GxEPD_BLACK);
    u8g2.setCursor(centerX-24, centerY+8);
    u8g2.print(text);
}

void drawWind(U8G2_FOR_ADAFRUIT_GFX &u8g2, int centerX, int centerY, int radius,const String text) {
    int startAngle = 135;
    int endAngle = 135;
    int angleRange = 270;

    for (int angle = 105; angle <= 165; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 3, GxEPD_BLACK);
    }
    for (int angle = 105; angle <= 165; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 2, GxEPD_WHITE);
    }
    if(text == "西南风"){
        for (int angle = 120; angle <= 150; angle++) {
            int x = centerX + radius * cos(radians(angle));
            int y = centerY + radius * sin(radians(angle));
            display.fillCircle(x, y, 3, GxEPD_BLACK);
        }
        drawRoundedTriangle(centerX, centerY, radius-5, 45, 1);    
    }
    
    for (int angle = 15; angle <= 75; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 3, GxEPD_BLACK);
    }
    for (int angle = 15; angle <= 75; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 2, GxEPD_WHITE);
    }
    
    if(text == "东南风"){
        for (int angle = 30; angle <= 60; angle++) {
            int x = centerX + radius * cos(radians(angle));
            int y = centerY + radius * sin(radians(angle));
            display.fillCircle(x, y, 3, GxEPD_BLACK);
        }
        drawRoundedTriangle(centerX, centerY, radius-5, 315, 1);   
    }

    for (int angle = 195; angle <= 255; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 3, GxEPD_BLACK);
    }
    for (int angle = 195; angle <= 255; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 2, GxEPD_WHITE);
    }
    if(text == "西北风"){
        for (int angle = 210; angle <= 240; angle++) {
            int x = centerX + radius * cos(radians(angle));
            int y = centerY + radius * sin(radians(angle));
            display.fillCircle(x, y, 3, GxEPD_BLACK);
        }
        drawRoundedTriangle(centerX, centerY, radius-5, 135, 1);   
    }
    

    for (int angle = 285; angle <= 345; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 3, GxEPD_BLACK);
    }
    for (int angle = 285; angle <= 345; angle++) {
        int x = centerX + radius * cos(radians(angle));
        int y = centerY + radius * sin(radians(angle));
        display.fillCircle(x, y, 2, GxEPD_WHITE);
    }
    if(text == "东北风"){
        for (int angle = 300; angle <= 330; angle++) {
            int x = centerX + radius * cos(radians(angle));
            int y = centerY + radius * sin(radians(angle));
            display.fillCircle(x, y, 3, GxEPD_BLACK);
        }
        drawRoundedTriangle(centerX, centerY, radius-5, 225, 1);
    }
    if(text == "西风"){
        drawRoundedTriangle(centerX, centerY, radius-5, 90, 1);   
    }
    if(text == "东风"){
        drawRoundedTriangle(centerX, centerY, radius-5, 270, 1);   
    }
    if(text == "南风"){
        drawRoundedTriangle(centerX, centerY, radius-5, 0, 1);   
    }
    if(text == "北风"){
        drawRoundedTriangle(centerX, centerY, radius-5, 180, 1);   
    }
    u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
    u8g2.setCursor(centerX-45, centerY+5);
    u8g2.print("西");
    u8g2.setCursor(centerX+35, centerY+5);
    u8g2.print("东");
    u8g2.setCursor(centerX-5, centerY+45);
    u8g2.print("南");
    u8g2.setCursor(centerX-5, centerY-35);
    u8g2.print("北");
}



void drawWeatherIcon(const std::string& weatherCondition, int xset , int yset , int w , int h) {
    static const std::map<std::string, const unsigned char*> weatherIcons = {
        {"晴", Bitmap_qt},
        {"阴", Bitmap_yt},
        {"多云", Bitmap_dy},
        {"少云", Bitmap_dy},
        {"阵雨", Bitmap_zheny},
        {"雷阵雨", Bitmap_lzy},
        {"雷阵雨伴有冰雹", Bitmap_lzybbb},
        {"小雨", Bitmap_xy},
        {"中雨", Bitmap_zhongy},
        {"大雨", Bitmap_dayu},
        {"暴雨", Bitmap_by},
        {"大暴雨", Bitmap_dby},
        {"特大暴雨", Bitmap_tdby},
        {"冻雨", Bitmap_dongy},
        {"雨夹雪", Bitmap_yjx},
        {"阵雪", Bitmap_zhenx},
        {"小雪", Bitmap_xx},
        {"中雪", Bitmap_zhongx},
        {"大雪", Bitmap_dx},
        {"暴雪", Bitmap_bx},
        {"浮尘", Bitmap_fc},
        {"扬沙", Bitmap_ys},
        {"沙尘暴", Bitmap_scb},
        {"雾", Bitmap_w},
        {"霾", Bitmap_m},
        {"风", Bitmap_f},
        {"飓风", Bitmap_jf},
        {"热带风暴", Bitmap_rdfb},
        {"龙卷风", Bitmap_ljf},
        {"未知", Bitmap_wz}
    };

    auto it = weatherIcons.find(weatherCondition);
    const unsigned char* bitmap = (it != weatherIcons.end()) ? it->second : Bitmap_wz;
    display.drawBitmap(xset, yset, bitmap, w, h, GxEPD_BLACK);
}


void drawDashedLine(int x0, int y0, int x1, int y1, int dashLength) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    float distance = sqrt(dx * dx + dy * dy);
    float dashCount = distance / dashLength;
    float xStep = dx / dashCount;
    float yStep = dy / dashCount;

    for (int i = 0; i < dashCount; i += 2) {
        int xStart = x0 + i * xStep;
        int yStart = y0 + i * yStep;
        int xEnd = x0 + (i + 1) * xStep;
        int yEnd = y0 + (i + 1) * yStep;
        display.drawLine(xStart, yStart, xEnd, yEnd, GxEPD_BLACK);
    }
}

void weather_epd(const Weather* weather) {
    display.setRotation(0); // 设置屏幕旋转
    display.setFullWindow();
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx = 0, tby; uint16_t tbw, tbh;

    do {
        display.fillScreen(GxEPD_WHITE);
        display.drawRoundRect(5, 10, 280, 190, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(12, 25);
        u8g2Fonts.print("当前天气");
        u8g2Fonts.setCursor(72, 25);
        u8g2Fonts.print(weather->location);

        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(12, 45);
        std::string obsTimeStr(weather->obsTime.c_str());
        display.print(obsTimeStr.substr(0, 10).c_str()); 

        drawWeatherIcon(weather->text.c_str(), 15 , 55 , 45 , 45);

        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(82, 95);
        display.print(weather->temp);
        display.setCursor(126, 95 - 20);
        display.print(".");
        display.getTextBounds(".", 0, 0, &tbx, &tby, &tbw, &tbh);
        display.setCursor(135 + tbw, 95);
        display.print("C");

        u8g2Fonts.setCursor(175, 75);
        u8g2Fonts.print(weather->text);

        u8g2Fonts.setCursor(175, 95);
        u8g2Fonts.print("体感温度: " + weather->feelsLike + "℃");

        u8g2Fonts.setCursor(12, 135);
        u8g2Fonts.print(weather->summary);

        const char* strings[] = {"质量", weather->windDir.c_str() ,"湿度", "能见度", "气压", "露点"};
        String strings_value[] = {weather->aqi, weather->windScale, weather->humidity, weather->vis + "公里", weather->pressure + "hPa", weather->dew + "℃"};
        int numStrings = sizeof(strings) / sizeof(strings[0]);

        for (int i = 0; i < numStrings; i++) {
            u8g2Fonts.setCursor(2 + tbx + 50 * i, 175);
            u8g2Fonts.print(strings[i]);
            u8g2Fonts.setCursor(2 + tbx + 50 * i, 195);
            u8g2Fonts.print(strings_value[i]);
            display.getTextBounds(strings[i], 0, 0, &tbx, &tby, &tbw, &tbh);
        }

    } while (display.nextPage());

    delay(100);
    display.setPartialWindow(0, 210, 400, 120);
    display.firstPage();
    String times[12];
    String tems[12];
    for (int i = 0; i < 12; i++) {
        tems[i] = weather->hours[i].temp;
        std::string timeStr(weather->hours[i].time.c_str());
        times[i] = timeStr.substr(11, 2).c_str(); 
    }

    do {
        display.fillScreen(GxEPD_WHITE);
        display.drawRoundRect(5, 210, 385, 90, 10, GxEPD_BLACK);
        int minTemp = 100, maxTemp = -100;
        std::vector<int> temps;

        for (int i = 0; i < 12; ++i) {
            int temp = tems[i].toInt();
            temps.push_back(temp);
            if (temp < minTemp) minTemp = temp;
            if (temp > maxTemp) maxTemp = temp;
        }

        int graphHeight = 55; 
        float scale = static_cast<float>(graphHeight) / (maxTemp - minTemp);

        int prevX = 20, prevY = 280 - static_cast<int>((temps[0] - minTemp) * scale);
        for (int i = 0; i < 12; i++) {
            int x = 15 + i * 33;
            int y = 280 - static_cast<int>((temps[i] - minTemp) * scale);
            display.fillCircle(x, y, 3, GxEPD_BLACK);

            u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
            u8g2Fonts.setCursor(x - 5, 295);
            u8g2Fonts.print(times[i]);

            u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
            u8g2Fonts.setCursor(x - 6, y - 4);
            u8g2Fonts.print(tems[i]);
            drawDashedLine(x, y + 2, x, 288, 5);
            if (i > 0) {
                display.drawLine(prevX, prevY, x, y, GxEPD_BLACK);
            }
            prevX = x;
            prevY = y;
        }
    } while (display.nextPage());

    delay(100);
    display.setPartialWindow(300, 10, 120, 190);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.drawRoundRect(300, 10, 90, 190, 10, GxEPD_BLACK);
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);

        for (int i = 0; i < 3; i++) {
            int yOffset = 60 * i;
            u8g2Fonts.setCursor(310, 22 + yOffset);
            u8g2Fonts.print(weather->days[i].fxDate);
            drawWeatherIcon(weather->days[i].textDay.c_str(), 312 , 22 + yOffset , 45 , 45);
            
            u8g2Fonts.setCursor(360, 42 + yOffset);
            u8g2Fonts.print(weather->days[i].tempMax + "℃");
            u8g2Fonts.setCursor(360, 58 + yOffset);
            u8g2Fonts.print(weather->days[i].tempMin + "℃");
            display.drawLine(310, 68 + yOffset, 380, 68 + yOffset, GxEPD_BLACK);
        }

    } while (display.nextPage());
}

void insights_epd(const Weather* weather) {
    display.setRotation(0); // 设置屏幕旋转
    display.setFullWindow();
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    do {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        display.drawRoundRect(5, 5, 125, 143, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(10, 20);
        u8g2Fonts.print("温度");
        drawGauge(u8g2Fonts, static_cast<int>(round((static_cast<float>(weather->temp.toInt()) / 65) * 100)), 65, 60, 40, weather->temp+"℃"); 
        u8g2Fonts.setCursor(10, 115);
        u8g2Fonts.print(weather->tempState);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        drawTextWithWrap(u8g2Fonts, 10, 130, 115, 2, conv.from_bytes(weather->tempText.c_str()));
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);


        display.drawRoundRect(135, 5, 125, 143, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(140, 20);
        u8g2Fonts.print("体感");
        drawGauge(u8g2Fonts, static_cast<int>(round((static_cast<float>(weather->feelsLike.toInt()) / 65) * 100)), 195, 60, 40, weather->feelsLike+"℃"); 
        u8g2Fonts.setCursor(140, 115);
        u8g2Fonts.print(weather->feelsLikeState);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        drawTextWithWrap(u8g2Fonts, 140, 130, 115, 2, conv.from_bytes(weather->feelsLikeText.c_str()));
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);


        display.drawRoundRect(265, 5, 125, 143, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(270, 20);
        u8g2Fonts.print("AQI");
        drawGauge(u8g2Fonts, static_cast<int>(round((static_cast<float>(weather->aqi.toInt()) / 500) * 100)), 325, 60, 40, weather->aqi); 
        u8g2Fonts.setCursor(270, 115);
        u8g2Fonts.print(weather->aqiState);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        drawTextWithWrap(u8g2Fonts, 270, 130, 115, 2, conv.from_bytes(weather->aqiText.c_str()));
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);


        display.drawRoundRect(5, 148, 125, 143, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(10, 163);
        u8g2Fonts.print("降水");
        drawPrecipitation(u8g2Fonts, 65, 203, 40, weather->rainAmount+"毫米");
        u8g2Fonts.setCursor(10, 258);
        u8g2Fonts.print(weather->precipitationState);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        drawTextWithWrap(u8g2Fonts, 10, 273, 115, 2, conv.from_bytes(weather->precipitationText.c_str()));
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);


        display.drawRoundRect(135, 148, 255, 143, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(140, 163);
        u8g2Fonts.print("风速");
        u8g2Fonts.setCursor(270, 175);
        u8g2Fonts.print("来自 " + weather->pvdrWindDir);
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(270, 205);
        display.print(weather->windSpd);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        u8g2Fonts.setCursor(300, 195);
        u8g2Fonts.print("公里/小时");
        u8g2Fonts.setCursor(300, 210);
        u8g2Fonts.print("风速");
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
        display.setCursor(270, 237);
        display.print(weather->windGust);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        u8g2Fonts.setCursor(300, 227);
        u8g2Fonts.print("公里/小时");
        u8g2Fonts.setCursor(300, 242);
        u8g2Fonts.print("阵风");
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);

        drawWind(u8g2Fonts, 195, 203, 40, weather->pvdrWindDir); 
        u8g2Fonts.setCursor(140, 258);
        u8g2Fonts.print(weather->windState);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        drawTextWithWrap(u8g2Fonts, 140, 273, 230, 2, conv.from_bytes(weather->windText.c_str()));
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);

        /*
        display.drawRoundRect(265, 148, 125, 143, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(270, 163);
        u8g2Fonts.print("湿度");
        u8g2Fonts.setCursor(270, 258);
        u8g2Fonts.print(weather->humidityState);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        drawTextWithWrap(u8g2Fonts, 270, 273, 115, conv.from_bytes(weather->humidityText.c_str()));
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
        */
        

        //display.drawRoundRect(5, 5, 90, 125, 10, GxEPD_BLACK);
    } while (display.nextPage());
    
}

void insights_epd2(const Weather* weather) {
    display.setRotation(0); // 设置屏幕旋转
    display.setFullWindow();
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    do {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        display.drawRoundRect(5, 5, 385, 143, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(10, 20);
        u8g2Fonts.print("室内温湿度");
        display.drawBitmap(10, 45, temp, 64, 64, GxEPD_BLACK);
        display.setFont(&FreeMonoBold12pt7b);
        drawGauge(u8g2Fonts, static_cast<int>(round((static_cast<float>(weather->shtTemp) / 65) * 100)), 105, 85, 40, ""); 
        u8g2Fonts.setCursor(105-18, 85+12);
        u8g2Fonts.print(String(weather->shtTemp) + "℃");
        u8g2Fonts.setCursor(105-53, 85+52);
        u8g2Fonts.print("室内温度");
        display.drawBitmap(140, 45, humidity, 64, 64, GxEPD_BLACK);
        drawGauge(u8g2Fonts, static_cast<int>(round((static_cast<float>(weather->shtHumidity) / 100) * 100)), 245, 85, 40, ""); 
        u8g2Fonts.setCursor(245-18, 85+12);
        u8g2Fonts.print(String(weather->shtHumidity) + "%");
        u8g2Fonts.setCursor(245-53, 85+52);
        u8g2Fonts.print("室内湿度");
        display.drawLine(310, 10, 310, 143, GxEPD_BLACK);
        Serial.print(mode);
        if (mode == 0) {
            display.drawBitmap(360, 5, wifi, 30, 30, GxEPD_BLACK);
            u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
            drawTextWithWrap(u8g2Fonts, 312, 40, 70, 2, conv.from_bytes("设备运行正常"));
            drawTextWithWrap(u8g2Fonts, 312, 80, 70, 6, conv.from_bytes(weather->hitokoto.c_str()));
            u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
        }else if(mode == 1){
            display.drawBitmap(360, 5, wifiClose, 30, 30, GxEPD_BLACK);
            u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
            drawTextWithWrap(u8g2Fonts, 312, 40, 70, 6, conv.from_bytes("设备未连接到可用网络,AP模式已经启动请访问192.168.4.1来配置设备"));
            u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
        }else if(mode == 2){
            display.drawBitmap(360, 5, wifi, 30, 30, GxEPD_BLACK);
            u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
            drawTextWithWrap(u8g2Fonts, 312, 40, 70, 6, conv.from_bytes("设备未配置完毕API,AP模式已经启动请访问192.168.4.1来配置设备"));
            u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
        }else{
            display.drawBitmap(360, 5, wifiClose, 30, 30, GxEPD_BLACK);
            u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
            drawTextWithWrap(u8g2Fonts, 312, 40, 70, 6, conv.from_bytes("设备未配置完毕API,AP模式已经启动请访问192.168.4.1来配置设备"));
            u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
        }
        float batteryLevelValue = batteryLevel();
        u8g2Fonts.setCursor(340, 20);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        u8g2Fonts.print(String((int)batteryLevelValue) + "%");
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
        if (batteryLevelValue >= 60) {
            display.drawBitmap(312, 10, Bitmap_bat3, 21, 12, GxEPD_BLACK);
        }else if(batteryLevelValue >= 30){
            display.drawBitmap(312, 10, Bitmap_bat2, 21, 12, GxEPD_BLACK);
        }else{
            display.drawBitmap(312, 10, Bitmap_bat1, 21, 12, GxEPD_BLACK);
        }

        display.drawRoundRect(5, 148, 125, 143, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(10, 163);
        u8g2Fonts.print("紫外线");
        drawGauge(u8g2Fonts, static_cast<int>(round((static_cast<float>(weather->uv.toInt()) / 15) * 100)), 65, 203, 40, weather->uv+"/UV"); 
        u8g2Fonts.setCursor(10, 258);
        u8g2Fonts.print(weather->uvState);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        drawTextWithWrap(u8g2Fonts, 10, 273, 115, 2, conv.from_bytes(weather->uvText.c_str()));
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);


        display.drawRoundRect(135, 148, 255, 143, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(140, 163);
        u8g2Fonts.print("湿度");
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(270, 205);
        display.print(weather->humidity);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        u8g2Fonts.setCursor(300, 195);
        u8g2Fonts.print("相当湿度");
        u8g2Fonts.setCursor(300, 210);
        u8g2Fonts.print("%");
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
        display.setCursor(270, 237);
        display.print(weather->dew);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        u8g2Fonts.setCursor(300, 227);
        u8g2Fonts.print("露点");
        u8g2Fonts.setCursor(300, 242);
        u8g2Fonts.print("℃");
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
        drawHumidity(u8g2Fonts, weather->humidity.toInt(), 195, 203, 60);
        u8g2Fonts.setCursor(140, 258);
        u8g2Fonts.print(weather->humidityState);
        u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312a);
        drawTextWithWrap(u8g2Fonts, 140, 273, 230, 2, conv.from_bytes(weather->humidityText.c_str()));
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
    } while (display.nextPage());
}

void test_epd() {
    display.setRotation(0); // 设置屏幕旋转
    display.setFullWindow();
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    do {
        //drawGauge(u8g2Fonts, 50, 65, 60, 40, "test"); 
        drawWind(u8g2Fonts, 65, 60, 40, "西南风"); 
    } while (display.nextPage());
}