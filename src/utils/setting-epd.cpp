#include "setting-epd.h"
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "Bitmap.cpp"
#include "weather.h"


void setting_epd() {
    display.setRotation(0); // 设置屏幕旋转
    display.setFullWindow();
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    do {
        u8g2Fonts.setCursor(10,20);
        u8g2Fonts.print("设置");
        u8g2Fonts.setCursor(367,20);
        u8g2Fonts.print("/10秒");
        u8g2Fonts.setCursor(110,280);
        u8g2Fonts.print("倒计时结束自动进入当前模式");

        display.drawBitmap(50, 60, Bitmap_qt, 45, 45, GxEPD_BLACK);
        display.drawRoundRect(40, 50, 65, 80, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(60,110);
        u8g2Fonts.print("天气");
        
        display.drawBitmap(130, 60, Bitmap_yt, 45, 45, GxEPD_BLACK);
        display.drawRoundRect(120, 50, 65, 80, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(140,110);
        u8g2Fonts.print("详情1");

        display.drawBitmap(210, 60, Bitmap_yt, 45, 45, GxEPD_BLACK);
        display.drawRoundRect(200, 50, 65, 80, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(220,110);
        u8g2Fonts.print("详情2");

        display.drawBitmap(290, 60, rili, 45, 45, GxEPD_BLACK);
        display.drawRoundRect(280, 50, 65, 80, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(300,110);
        u8g2Fonts.print("日历");

        display.drawBitmap(50, 170, wifi45, 45, 45, GxEPD_BLACK);
        display.drawRoundRect(40, 160, 65, 80, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(50,220);
        u8g2Fonts.print("AP模式");

        display.drawBitmap(130, 170, Bitmap_qt, 45, 45, GxEPD_BLACK);
        display.drawRoundRect(120, 160, 65, 80, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(125,220);
        u8g2Fonts.print("天气模式");

        display.drawBitmap(210, 170, Bitmap_wz, 45, 45, GxEPD_BLACK);
        display.drawRoundRect(200, 160, 65, 80, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(210,220);
        u8g2Fonts.print("未开发");

        display.drawBitmap(290, 170, Bitmap_wz, 45, 45, GxEPD_BLACK);
        display.drawRoundRect(280, 160, 65, 80, 10, GxEPD_BLACK);
        u8g2Fonts.setCursor(290,220);
        u8g2Fonts.print("未开发");

    } while (display.nextPage());
}

void pointer_epd(int linex) {
    if(linex == 4){
        display.setPartialWindow(0, 132, 400, 25);
        display.firstPage();
        do {
            display.fillRect(0, 132, 400, 25, GxEPD_WHITE); // 清除指定区域
        } while (display.nextPage());
        
    }
    if(linex == 0){
        display.setPartialWindow(0, 240, 400, 25);
        display.firstPage();
        do {
            display.fillRect(0, 240, 400, 25, GxEPD_WHITE); // 清除指定区域
        } while (display.nextPage());
    }
    int x = 60; 
    int y = 132; 
    int horizontalMove = 80; 
    int verticalMove = 108; 
    int itemsPerRow = 4; 
    int liney = linex / itemsPerRow; 
    linex = linex % itemsPerRow;
    Serial.println(liney);
    y = y+liney*verticalMove;
    

    display.setPartialWindow(0, y, 400, 25);
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    
    do {
        display.drawBitmap(x+linex*horizontalMove, y, pointer, 25, 25, GxEPD_BLACK);
    } while (display.nextPage());

}

void times_epd(int time) {
    display.setPartialWindow(355, 0, 6, 25);
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    do {
        u8g2Fonts.setCursor(355,20);
        u8g2Fonts.print(String(time));
    } while (display.nextPage());
}

void start_epd(){
    display.setRotation(0); // 设置屏幕旋转
    display.setFullWindow();
    display.firstPage();
    u8g2Fonts.begin(display);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a); // 设置中文字体
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // 设置文字颜色为黑色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // 设置背景颜色为白色
    display.setTextColor(GxEPD_BLACK);
    
    do {
        display.drawBitmap(0, -60, fulan, 400, 320, GxEPD_BLACK);
        
    } while (display.nextPage());
    
}

void startText_epd(String text){
    display.setPartialWindow(0, 280, 400, 20);
    display.firstPage();
    do {
        u8g2Fonts.setCursor(130,295);
        u8g2Fonts.print(text);
    } while (display.nextPage());
}