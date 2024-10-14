#ifndef _WEATHER_H_
#define _WEATHER_H_

#include <Preferences.h>

// 天气配置结构体
struct WeatherConfig {
    String locationId; // 地区ID
    String apiKey;     // API密钥
    String mscApiKey;  // API密钥
    String lat;        // 纬度
    String lon;        // 经度
    String adm1;       // 一级行政区
    String adm2;       // 二级行政区
    String name;       // 地名
    String loca;       // 地名
};

// 每日天气结构体
struct DayWeather {
    String fxDate;    // 日期
    String temp;      // 温度
    String tempMax;   // 最高温度
    String tempMin;   // 最低温度
    String textDay;   // 白天天气
    String textNight; // 夜晚天气
};

// 每小时天气结构体
struct HourWeather {
    String time; // 时间（小时）
    String temp; // 温度
    String text; // 天气
};

// 综合天气结构体
struct Weather {
    String location;  // 地点
    String obsTime;   // 观测时间
    String time;      // 时间
    String tempMax;   // 最高温度
    String tempMin;   // 最低温度
    String temp;      // 温度
    String feelsLike; // 体感温度
    String aqi;       // 空气质量指数
    String windDir;   // 风向
    String windScale; // 风级
    String humidity;  // 湿度
    String pressure;  // 气压
    String dew;       // 露点
    String vis;       // 能见度
    String uv;        // 紫外线
    String text;      // 天气描述
    String summary; //天气简述
    String windGust; //阵风
    String pvdrWindDir; //风向
    String windSpd; //风速
    float shtTemp; //室温
    float shtHumidity; //室内湿度
    String hitokoto; //一言

    String tempText; //温度简述
    String tempState; //温度状态
    String windText; //风速简述
    String windState; //风速状态
    String cloudText; //云量简述
    String cloudState; //云量状态
    String humidityText; //湿度简述
    String humidityState; //湿度状态
    String visibilityText; //能见度简述
    String visibilityState; //能见度状态
    String pressureText; //气压简述
    String pressureState; //气压状态
    String feelsLikeText; //体感简述
    String feelsLikeState; //体感状态
    String uvText; //紫外线简述
    String uvState; //紫外线状态
    String precipitationText; //降水简述
    String precipitationState; //降水状态
    String aqiText; //空气质量简述
    String aqiState; //空气质量状态
    String rainAmount; //降水量

    DayWeather days[3];   // 3天天气预报
    HourWeather hours[24];// 24小时天气预报
};

extern Weather weather;
constexpr char APIKEY[] = "49c460fe75c14629a4e2ada12626fbaa";
constexpr char LOCA[] = "beijin";

extern Preferences preferences;
Weather getWeatherAll();
void writeWeather(const Weather* weather);
Weather readWeather();
void updateSHT();
void saveWeatherConfig(const WeatherConfig* config);

#endif 

