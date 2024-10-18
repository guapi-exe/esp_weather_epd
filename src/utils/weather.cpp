#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "weather.h"
#include <ArduinoUZlib.h>
#include <FS.h>
#include <SPIFFS.h>
#include "sht.h"
#include "main.h"

void saveWeatherConfig(const WeatherConfig* config) {
    preferences.begin("weather-config", false);
    if (config->locationId != nullptr) {
        preferences.putString("locationId", config->locationId);
    }
    if (config->apiKey != nullptr) {
        preferences.putString("apiKey", config->apiKey);
    }
    if (config->mscApiKey != nullptr) {
        preferences.putString("mscApiKey", config->mscApiKey);
    }
    if (config->lat != nullptr) {
        preferences.putString("lat", config->lat);
    }
    if (config->lon != nullptr) {
        preferences.putString("lon", config->lon);
    }
    if (config->adm1 != nullptr) {
        preferences.putString("adm1", config->adm1);
    }
    if (config->adm2 != nullptr) {
        preferences.putString("adm2", config->adm2);
    }
    if (config->name != nullptr) {
        preferences.putString("name", config->name);
    }
    if (config->loca != nullptr) {
        preferences.putString("loca", config->loca);
    }
    preferences.end();
}

WeatherConfig readWeatherConfig() {
    WeatherConfig config;
    preferences.begin("weather-config", true);
    config.locationId = preferences.getString("locationId", "");
    config.apiKey = preferences.getString("apiKey", "");
    config.mscApiKey = preferences.getString("mscApiKey", "");
    config.lat = preferences.getString("lat", "");
    config.lon = preferences.getString("lon", "");
    config.adm1 = preferences.getString("adm1", "");
    config.adm2 = preferences.getString("adm2", "");
    config.name = preferences.getString("name", "");
    config.loca = preferences.getString("loca", "");
    preferences.end();
    return config;
}

WeatherConfig getLocationId(const char* loca, const char* apiKey) {
    WeatherConfig config;
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); 
        HTTPClient http;
        String url = "https://geoapi.qweather.com/v2/city/lookup?location=" + String(loca) + "&key=" + String(apiKey);
        http.begin(url);
        http.addHeader("Accept-Encoding", "gzip");

        int httpCode = http.GET();
        if (httpCode > 0) {
            // Read the compressed payload
            WiFiClient* stream = http.getStreamPtr();
            size_t size = stream->available();
            uint8_t inbuff[size];
            stream->readBytes(inbuff, size);

            // Decompress the payload
            uint8_t* outbuf = NULL;
            uint32_t outsize = 0;
            int result = ArduinoUZlib::decompress(inbuff, size, outbuf, outsize);
            if (result != 0) {
                String payload = String((char*)outbuf);
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, payload);
                if (!error) {
                    JsonObject day = doc["location"].as<JsonArray>()[0];
                    config.locationId = day["id"].as<String>();
                    config.lat = day["lat"].as<String>();
                    config.lon = day["lon"].as<String>();
                    config.adm1 = day["adm1"].as<String>();
                    config.adm2 = day["adm2"].as<String>();
                    config.name = day["name"].as<String>();
                    config.apiKey = APIKEY;
                } else {
                    Serial.println("Error deserializing JSON");
                }
            } else {
                Serial.println("Error decompressing data");
            }
            free(outbuf); 
        } else {
            Serial.println("Error on HTTP request");
        }
        http.end();
    }
    return config;
}

void getWeatherData(const char* location, const char* apiKey) { // 弃用
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); 
        HTTPClient http;
        String url = "https://devapi.qweather.com/v7/weather/now?location=" + String(location) + "&key=" + String(apiKey);
        
        http.begin(url);
        http.addHeader("Accept-Encoding", "gzip");
        int httpCode = http.GET();
        
        if (httpCode > 0) {
            // Read the compressed payload
            WiFiClient* stream = http.getStreamPtr();
            size_t size = stream->available();
            uint8_t inbuff[size];
            stream->readBytes(inbuff, size);

            // Decompress the payload
            uint8_t* outbuf = NULL;
            uint32_t outsize = 0;
            int result = ArduinoUZlib::decompress(inbuff, size, outbuf, outsize);
            String payload = http.getString();
            if (result != 0) {
                String payload = String((char*)outbuf);
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, payload);
                Serial.println(doc["now"].as<String>());
                if (!error) {
                    weather.obsTime = doc["now"]["obsTime"].as<String>();
                    weather.temp = doc["now"]["temp"].as<String>();
                    weather.feelsLike = doc["now"]["feelsLike"].as<String>();
                    weather.windDir = doc["now"]["windDir"].as<String>();
                    weather.windScale = doc["now"]["windScale"].as<String>();
                    weather.humidity = doc["now"]["humidity"].as<String>();
                    weather.pressure = doc["now"]["pressure"].as<String>();
                    weather.dew = doc["now"]["dew"].as<String>();
                    weather.vis = doc["now"]["vis"].as<String>();
                    weather.text = doc["now"]["text"].as<String>();
                    Serial.println(weather.temp);
                }else {
                    Serial.println("Error deserializing JSON");
                }
            }
            free(outbuf); 
        } else {
            Serial.println("Error on HTTP request");
        }

        http.end();
    }
}

void getAQIData(const WeatherConfig* config) { //弃用
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); 
        HTTPClient http;
        String url = "https://devapi.qweather.com/airquality/v1/current/" + config->lat + "/" + config->lon + "?key=" + config->apiKey;
        http.begin(url);
        http.addHeader("Accept-Encoding", "gzip");
        int httpCode = http.GET();

        if (httpCode > 0) {
            // Read the compressed payload
            WiFiClient* stream = http.getStreamPtr();
            size_t size = stream->available();
            uint8_t inbuff[size];
            stream->readBytes(inbuff, size);

            // Decompress the payload
            uint8_t* outbuf = NULL;
            uint32_t outsize = 0;
            int result = ArduinoUZlib::decompress(inbuff, size, outbuf, outsize);
            String payload = http.getString();
            if (result != 0) {
                String payload = String((char*)outbuf);
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, payload);
                if (!error) {
                    JsonObject indexes = doc["indexes"].as<JsonArray>()[0];
                    weather.aqi = indexes["aqi"].as<String>();
                }else {
                    Serial.println("Error deserializing JSON");
                }
            }
            free(outbuf); 
        } else {
            Serial.println("Error on HTTP request");
        }

        http.end();
    }
}

void getForecastWeatherData(const char* location, const char* apiKey) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); 
        HTTPClient http;
        String url = "https://devapi.qweather.com/v7/weather/3d?location=" + String(location) + "&key=" + String(apiKey);
        http.begin(url);
        http.addHeader("Accept-Encoding", "gzip");
        int httpCode = http.GET();

        if (httpCode > 0) {
            WiFiClient* stream = http.getStreamPtr();
            size_t size = stream->available();
            uint8_t inbuff[size];
            stream->readBytes(inbuff, size);

            uint8_t* outbuf = NULL;
            uint32_t outsize = 0;
            int result = ArduinoUZlib::decompress(inbuff, size, outbuf, outsize);
            String payload = http.getString();
            if (result != 0) {
                String payload = String((char*)outbuf);
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, payload);
                if (!error) {
                    JsonArray daily = doc["daily"].as<JsonArray>();
                    weather.tempMax = daily[0]["tempMax"].as<String>();
                    weather.tempMin = daily[0]["tempMin"].as<String>();
                    for (int i = 0; i < 3; i++) {
                        if(i == 0 ){weather.text = daily[i]["textDay"].as<String>();}
                        weather.days[i].fxDate = daily[i]["fxDate"].as<String>();
                        weather.days[i].tempMax = daily[i]["tempMax"].as<String>();
                        weather.days[i].tempMin = daily[i]["tempMin"].as<String>();
                        weather.days[i].textDay = daily[i]["textDay"].as<String>();
                        weather.days[i].textNight = daily[i]["textNight"].as<String>();
                    }
                }else {
                    Serial.println("Error deserializing JSON");
                }
            }
            free(outbuf); 
        } else {
            Serial.println("Error on HTTP request");
        }

        http.end();
    }
}

void getTodayWeatherData(const char* location, const char* apiKey) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); 
        HTTPClient http;
        String url = "https://devapi.qweather.com/v7/weather/24h?location=" + String(location) + "&key=" + String(apiKey);
        http.begin(url);
        http.addHeader("Accept-Encoding", "gzip");
        int httpCode = http.GET();
        if (httpCode > 0) {
            WiFiClient* stream = http.getStreamPtr();
            size_t size = stream->available();
            uint8_t inbuff[size];
            stream->readBytes(inbuff, size);

            uint8_t* outbuf = NULL;
            uint32_t outsize = 0;
            int result = ArduinoUZlib::decompress(inbuff, size, outbuf, outsize);
            String payload = http.getString();
            
            if (result != 0) {
                String payload = String((char*)outbuf);
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, payload);
                if (!error) {
                    JsonArray hourly = doc["hourly"].as<JsonArray>();
                    for (int i = 0; i < 24; i++) {
                        weather.hours[i].time = hourly[i]["fxTime"].as<String>();
                        weather.hours[i].temp = hourly[i]["temp"].as<String>();
                        weather.hours[i].text = hourly[i]["text"].as<String>();
                    }
                }else {
                    Serial.println("Error deserializing JSON");
                }
            }
            free(outbuf); 
        } else {
            Serial.println("Error on HTTP request");
        }

        http.end();
    }
}

void getHitokoto() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); 
        HTTPClient http;
        String url = "https://v1.hitokoto.cn/?c=f&encode=text";
        http.begin(url);
        int httpCode = http.GET();
        if (httpCode > 0) {
            String payload = http.getString();
            weather.hitokoto = payload;
        } else {
            Serial.println("Error on HTTP request");
        }

        http.end();
    }
}
void getWeatherDataNew(const WeatherConfig* config) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); 
        HTTPClient http;
        String url = "https://api.msn.cn/weather/overview?apikey="+config->mscApiKey+"&ocid=msftweather&cm=zh-cn&it=edgeid&scn=APP_ANON&wrapodata=false&includemapsmetadata=true&cuthour=true&lifeDays=2&lifeModes=18&includestorm=true&includeLifeActivity=true&lifeSubTypes=1%2C3%2C4%2C10%2C26&insights=65536&distanceinkm=0&regionDataCount=20&orderby=distance&days=3&pageOcid=prime-weather%3A%3Aweathertoday-peregrine&source=weather_csr&region=cn&market=zh-cn&locale=zh-cn&lat="+config->lat+"&lon=" + config->lon;
        http.begin(url);
        http.setTimeout(10000); 
        int httpCode = http.GET();
        Serial.print("Free heap: ");
        Serial.println(ESP.getFreeHeap());

        if (httpCode > 0) {
            WiFiClient* stream = http.getStreamPtr();
            DynamicJsonDocument doc(16000);
            StaticJsonDocument<20000> filter;
            filter["responses"][0]["weather"][0]["forecast"]["days"][0]["daily"] = true;
            filter["responses"][0]["weather"][0]["forecast"]["days"][0]["hourly"] = true;
            filter["responses"][0]["weather"][0]["insights"]["insights"] = true;
            filter["responses"][0]["weather"][0]["current"] = true;

            DeserializationError error = deserializeJson(doc, *stream, DeserializationOption::Filter(filter), DeserializationOption::NestingLimit(20));
            if (!error) {
                JsonArray summaries = doc["responses"][0]["weather"][0]["forecast"]["days"][0]["daily"]["day"]["summaries"].as<JsonArray>();
                JsonArray insights = doc["responses"][0]["weather"][0]["insights"]["insights"].as<JsonArray>();
                weather.aqi = doc["responses"][0]["weather"][0]["current"]["aqi"].as<String>();
                weather.windGust = doc["responses"][0]["weather"][0]["current"]["windGust"].as<String>();
                weather.pvdrWindDir = doc["responses"][0]["weather"][0]["current"]["pvdrWindDir"].as<String>();
                weather.windSpd = doc["responses"][0]["weather"][0]["current"]["windSpd"].as<String>();

                weather.obsTime = doc["responses"][0]["weather"][0]["current"]["created"].as<String>();
                weather.temp = doc["responses"][0]["weather"][0]["current"]["temp"].as<String>();
                weather.feelsLike = doc["responses"][0]["weather"][0]["current"]["feels"].as<String>();
                weather.windDir = doc["responses"][0]["weather"][0]["current"]["pvdrWindDir"].as<String>();
                weather.windScale = doc["responses"][0]["weather"][0]["current"]["pvdrWindSpd"].as<String>();
                weather.humidity = insights[3]["parameters"]["humidity"].as<String>();
                weather.pressure = doc["responses"][0]["weather"][0]["current"]["baro"].as<String>();
                weather.dew = doc["responses"][0]["weather"][0]["current"]["dewPt"].as<String>();
                weather.vis = doc["responses"][0]["weather"][0]["current"]["vis"].as<String>();
                weather.uv = doc["responses"][0]["weather"][0]["forecast"]["days"][0]["daily"]["uv"].as<String>();
                //weather.text = doc["responses"][0]["weather"][0]["forecast"]["days"][0]["daily"]["day"]["cap"].as<String>();

                weather.summary = summaries[0].as<String>() + summaries[1].as<String>();
                weather.tempText = insights[0]["teaserText"].as<String>();
                weather.tempState = insights[0]["headlineText"].as<String>();
                weather.windText = insights[1]["teaserText"].as<String>();
                weather.windState = insights[1]["headlineText"].as<String>();
                weather.cloudText = insights[2]["teaserText"].as<String>();
                weather.cloudState = insights[2]["headlineText"].as<String>();
                weather.humidityText = insights[3]["teaserText"].as<String>();
                weather.humidityState = insights[3]["headlineText"].as<String>();
                weather.visibilityText = insights[6]["teaserText"].as<String>();
                weather.visibilityState = insights[6]["headlineText"].as<String>();
                weather.pressureText = insights[7]["teaserText"].as<String>();
                weather.pressureState = insights[7]["headlineText"].as<String>();
                weather.feelsLikeText = insights[8]["teaserText"].as<String>();
                weather.feelsLikeState = insights[8]["headlineText"].as<String>();
                weather.uvText = insights[9]["teaserText"].as<String>();
                weather.uvState = insights[9]["headlineText"].as<String>();
                weather.precipitationText = insights[10]["teaserText"].as<String>();
                weather.precipitationState = insights[10]["headlineText"].as<String>();
                weather.rainAmount = insights[10]["parameters"]["rain_amount"].as<String>();
                weather.aqiText = insights[11]["teaserText"].as<String>();
                weather.aqiState = insights[11]["headlineText"].as<String>();
            } else {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
            }
        } else {
            Serial.println("Error on HTTP request");
        }

        http.end();
    }
}

Weather readWeather() {
    File file = SPIFFS.open("/weatherData.json", FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return weather;
    }

    String weatherJson = file.readString();
    file.close();

    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, weatherJson);
    if (error) {
        Serial.print("JSON Deserialization Error: ");
        Serial.println(error.c_str());
        return weather;
    }

    weather.location = doc["location"].as<String>();
    weather.obsTime = doc["obsTime"].as<String>();
    weather.time = doc["time"].as<String>();
    weather.tempMax = doc["tempMax"].as<String>();
    weather.tempMin = doc["tempMin"].as<String>();
    weather.temp = doc["temp"].as<String>();
    weather.feelsLike = doc["feelsLike"].as<String>();
    weather.aqi = doc["aqi"].as<String>();
    weather.windDir = doc["windDir"].as<String>();
    weather.windScale = doc["windScale"].as<String>();
    weather.humidity = doc["humidity"].as<String>();
    weather.pressure = doc["pressure"].as<String>();
    weather.dew = doc["dew"].as<String>();
    weather.vis = doc["vis"].as<String>();
    weather.uv = doc["uv"].as<String>();
    weather.text = doc["text"].as<String>();
    weather.summary = doc["summary"].as<String>();
    weather.windGust = doc["windGust"].as<String>();
    weather.pvdrWindDir = doc["pvdrWindDir"].as<String>();
    weather.windSpd = doc["windSpd"].as<String>();
    weather.shtTemp = doc["shtTemp"];
    weather.shtHumidity = doc["shtHumidity"];
    weather.hitokoto = doc["hitokoto"].as<String>();
    weather.tempText = doc["tempText"].as<String>();
    weather.tempState = doc["tempState"].as<String>();
    weather.windText = doc["windText"].as<String>();
    weather.windState = doc["windState"].as<String>();
    weather.cloudText = doc["cloudText"].as<String>();
    weather.cloudState = doc["cloudState"].as<String>();
    weather.humidityText = doc["humidityText"].as<String>();
    weather.humidityState = doc["humidityState"].as<String>();
    weather.visibilityText = doc["visibilityText"].as<String>();
    weather.visibilityState = doc["visibilityState"].as<String>();
    weather.pressureText = doc["pressureText"].as<String>();
    weather.pressureState = doc["pressureState"].as<String>();
    weather.feelsLikeText = doc["feelsLikeText"].as<String>();
    weather.feelsLikeState = doc["feelsLikeState"].as<String>();
    weather.uvText = doc["uvText"].as<String>();
    weather.uvState = doc["uvState"].as<String>();
    weather.precipitationText = doc["precipitationText"].as<String>();
    weather.precipitationState = doc["precipitationState"].as<String>();
    weather.aqiText = doc["aqiText"].as<String>();
    weather.aqiState = doc["aqiState"].as<String>();
    weather.rainAmount = doc["rainAmount"].as<String>();

    for (int i = 0; i < 3; i++) {
        weather.days[i].fxDate = doc["days"][i]["fxDate"].as<String>();
        weather.days[i].temp = doc["days"][i]["temp"].as<String>();
        weather.days[i].tempMax = doc["days"][i]["tempMax"].as<String>();
        weather.days[i].tempMin = doc["days"][i]["tempMin"].as<String>();
        weather.days[i].textDay = doc["days"][i]["textDay"].as<String>();
        weather.days[i].textNight = doc["days"][i]["textNight"].as<String>();
    }

    for (int i = 0; i < 24; i++) {
        weather.hours[i].time = doc["hours"][i]["time"].as<String>();
        weather.hours[i].temp = doc["hours"][i]["temp"].as<String>();
        weather.hours[i].text = doc["hours"][i]["text"].as<String>();
    }

    return weather;
}

void writeWeather(const Weather* weather) {
    StaticJsonDocument<4096> doc;
    doc["location"] = weather->location;
    doc["obsTime"] = weather->obsTime;
    doc["time"] = weather->time;
    doc["tempMax"] = weather->tempMax;
    doc["tempMin"] = weather->tempMin;
    doc["temp"] = weather->temp;
    doc["feelsLike"] = weather->feelsLike;
    doc["aqi"] = weather->aqi;
    doc["windDir"] = weather->windDir;
    doc["windScale"] = weather->windScale;
    doc["humidity"] = weather->humidity;
    doc["pressure"] = weather->pressure;
    doc["dew"] = weather->dew;
    doc["vis"] = weather->vis;
    doc["uv"] = weather->uv;
    doc["text"] = weather->text;
    doc["summary"] = weather->summary;
    doc["windGust"] = weather->windGust;
    doc["pvdrWindDir"] = weather->pvdrWindDir;
    doc["windSpd"] = weather->windSpd;
    doc["shtTemp"] = weather->shtTemp;
    doc["shtHumidity"] = weather->shtHumidity;
    doc["hitokoto"] = weather->hitokoto;
    doc["tempText"] = weather->tempText;
    doc["tempState"] = weather->tempState;
    doc["windText"] = weather->windText;
    doc["windState"] = weather->windState;
    doc["cloudText"] = weather->cloudText;
    doc["cloudState"] = weather->cloudState;
    doc["humidityText"] = weather->humidityText;
    doc["humidityState"] = weather->humidityState;
    doc["visibilityText"] = weather->visibilityText;
    doc["visibilityState"] = weather->visibilityState;
    doc["pressureText"] = weather->pressureText;
    doc["pressureState"] = weather->pressureState;
    doc["feelsLikeText"] = weather->feelsLikeText;
    doc["feelsLikeState"] = weather->feelsLikeState;
    doc["uvText"] = weather->uvText;
    doc["uvState"] = weather->uvState;
    doc["precipitationText"] = weather->precipitationText;
    doc["precipitationState"] = weather->precipitationState;
    doc["aqiText"] = weather->aqiText;
    doc["aqiState"] = weather->aqiState;
    doc["rainAmount"] = weather->rainAmount;

    for (int i = 0; i < 3; i++) {
        doc["days"][i]["fxDate"] = weather->days[i].fxDate;
        doc["days"][i]["temp"] = weather->days[i].temp;
        doc["days"][i]["tempMax"] = weather->days[i].tempMax;
        doc["days"][i]["tempMin"] = weather->days[i].tempMin;
        doc["days"][i]["textDay"] = weather->days[i].textDay;
        doc["days"][i]["textNight"] = weather->days[i].textNight;
    }

    for (int i = 0; i < 24; i++) {
        doc["hours"][i]["time"] = weather->hours[i].time;
        doc["hours"][i]["temp"] = weather->hours[i].temp;
        doc["hours"][i]["text"] = weather->hours[i].text;
    }

    String weatherJson;
    serializeJson(doc, weatherJson);
    File file = SPIFFS.open("/weatherData.json", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.print(weatherJson);
    file.close();
}

void updateSHT(){
    TempHumidity TH = getTempH();
    weather.shtTemp = TH.temperature;
    weather.shtHumidity = TH.humidity;
}

Weather getWeatherAll() {
    WeatherConfig config = readWeatherConfig();
    const char* apiKey = config.apiKey.c_str();
    const char* mscApiKey = config.mscApiKey.c_str();
    if(mode == 0){//mode 0 网络连接成功
        if(config.loca == ""){config.loca = LOCA;saveWeatherConfig(&config);}
        const char* loca = config.loca.c_str();
        if (config.locationId == "") {
            config = getLocationId(loca, apiKey); 
            saveWeatherConfig(&config);
        }
        const char* location = config.locationId.c_str();
        weather.location = String(config.adm1) + "," + String(config.adm2) + "," + String(config.name);
        if(config.apiKey == "" || config.mscApiKey == ""){
            mode = 2; //mode 2网络连接成功未配置api
        }
        if(config.apiKey != ""){
            getForecastWeatherData(location, apiKey);
            delay(100);
            getTodayWeatherData(location, apiKey);
            delay(100);
        }
        if(config.mscApiKey != ""){
            getWeatherDataNew(&config);
            delay(100);
        }
    }
    
    updateSHT();
    delay(100);
    getHitokoto();
    delay(100);
    return weather;
}

