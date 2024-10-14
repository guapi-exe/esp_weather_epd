#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "webserver.h"
#include "wifi.h"
#include "esp_task_wdt.h"
#include "weather.h"

const char* PARAM_MESSAGE = "message";
AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void initWebServer() {
    server.serveStatic("/", SPIFFS, "/")
        .setDefaultFile("test.html")
        .setCacheControl("max-age=600");
    server.on("/api/setWifi", HTTP_POST, [] (AsyncWebServerRequest *request) {
        String ssid = "";
        String password = "";

        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
            ssid = request->getParam("ssid", true)->value();
            password = request->getParam("password", true)->value();

            WiFi.begin(ssid.c_str(), password.c_str());

            while (WiFi.status() != WL_CONNECTED) {
                delay(1000);
                Serial.println("Connecting to new WiFi...");
            }
            saveWiFiConfig(ssid.c_str(), password.c_str());
            Serial.println("Connected to new WiFi");
            request->send(200, "text/plain", "WiFi credentials updated successfully");
        } else {
            request->send(400, "text/plain", "Missing ssid or password");
        }
    });
    server.on("/api/setData", HTTP_POST, [] (AsyncWebServerRequest *request) {
        String hfApi = "";
        String msnApi = "";
        String location = "";

        if (request->hasParam("hfApi", true) && request->hasParam("msnApi", true) && request->hasParam("location", true)) {
            hfApi = request->getParam("hfApi", true)->value();
            msnApi = request->getParam("msnApi", true)->value();
            location = request->getParam("location", true)->value();
            WeatherConfig config;
            config.apiKey = hfApi;
            config.mscApiKey = msnApi;
            
            saveWeatherConfig(&config);
            request->send(200, "text/plain", "WiFi credentials updated successfully");
        } else {
            request->send(400, "text/plain", "Missing ssid or password");
        }
    });
    
    server.on("/scan", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("HTTP get /scan");
        try {
            WiFi.scanNetworks(true ,false ,false ,1000);
            while (WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
                delay(50);
                esp_task_wdt_reset();
            }
            int n = WiFi.scanComplete();
            if (n >= 0) {
                String json = "[";
                for (int i = 0; i < n; ++i) {
                    if (i) json += ",";
                    json += "{";
                    json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
                    json += "\"rssi\":" + String(WiFi.RSSI(i));
                    json += "}";
                }
                json += "]";
                Serial.println(json);
                request->send(200, "application/json", json);
            } else {
                request->send(500, "text/plain", "Scan failed");
                Serial.println("Scan failed");
            }
        } catch (const std::exception& e) { 
            request->send(500, "text/plain", "Internal Server Error");
            Serial.println(e.what());
        }
    });

    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("HTTP get /get");
        request->send(200, "text/plain", "Hello, GET: ");
    });

    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    dnsServer.start(53, "*", WiFi.softAPIP());
    server.begin();
    Serial.print("Server Open");
}