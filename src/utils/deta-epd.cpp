#include<iostream>
#include "deta-epd.h"
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "Bitmap.cpp"
#include "weather.h"
 
int weekday(int y, int m, int d) {
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    int k = y % 100;
    int j = y / 100;
    int week = (d + 13 * (m + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
    return (week + 5) % 7; 
}



void deta_epd(const Weather* weather){
    std::string obsTimeStr(weather->obsTime.c_str());
    int year = std::stoi(obsTimeStr.substr(0, 4));
    int month = std::stoi(obsTimeStr.substr(5, 2));
    int day = std::stoi(obsTimeStr.substr(8, 2));
    int week = weekday(year, month, 1);
    int maxday = 30;
    if((year % 4 == 0 && month == 2)|| (year%400 == 0 && month == 2)){
        maxday = 29;
    }else if(month == 2){
        maxday = 28;
    }else if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12){
        maxday = 31;
    }else{
        maxday = 30;
    }
    Serial.print(maxday);
    Serial.print(week);
    Serial.print(day);
    Serial.print(month);
    Serial.print(year);

    display.setRotation(0); // 设置屏幕旋转
    display.setFullWindow();
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    int y = 1;
    do {
        display.drawBitmap(5, 5, rili, 45, 45, GxEPD_BLACK);
        display.drawLine(55,0,55,300, GxEPD_BLACK);
        display.setFont(&FreeMonoBold9pt7b);
        display.setCursor(80, 20);
        display.print(obsTimeStr.substr(0, 10).c_str()); 
        u8g2Fonts.setForegroundColor(GxEPD_WHITE);
        u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
        display.fillRect(70, 35, 310, 25, GxEPD_BLACK); 
        u8g2Fonts.setCursor(80,50);
        u8g2Fonts.print("一");
        display.drawLine(70,50,70,300, GxEPD_BLACK);
        u8g2Fonts.setCursor(125,50);
        u8g2Fonts.print("二");
        display.drawLine(110,50,110,300, GxEPD_BLACK);
        u8g2Fonts.setCursor(170,50);
        u8g2Fonts.print("三");
        display.drawLine(155,50,155,300, GxEPD_BLACK);
        u8g2Fonts.setCursor(215,50);
        u8g2Fonts.print("四");
        display.drawLine(200,50,200,300, GxEPD_BLACK);
        u8g2Fonts.setCursor(260,50);
        u8g2Fonts.print("五");
        display.drawLine(245,50,245,300, GxEPD_BLACK);
        u8g2Fonts.setCursor(305,50);
        u8g2Fonts.print("六");
        display.drawLine(290,50,290,300, GxEPD_BLACK);
        u8g2Fonts.setCursor(350,50);
        u8g2Fonts.print("日");
        display.drawLine(335,50,335,300, GxEPD_BLACK);
        display.drawLine(379,50,379,300, GxEPD_BLACK);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        for (int i = 1; i <= maxday; i++) {
            u8g2Fonts.setCursor(80+week*45,50+y*40);
            u8g2Fonts.print(i);
            if(i == day){
                display.drawRoundRect(73+week*45, 35+y*40, 25, 25, 5, GxEPD_BLACK);
            }
            if(week >= 6){week = 0;y++;}
            else{week++;} 
        }
    } while (display.nextPage());
}
