#pragma once

#ifndef PERSONALSERVER_H
#define PERSONALSERVER_H

#include <WiFi.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <WebServer.h>

#include <FS.h>
#include <SPIFFS.h>

#include <GyverPortal.h>

#include <ArduinoJson.h>

#define ESP_DRD_USE_SPIFFS true
#define JSON_CONFIG_FILE "/test_config.json"

#ifdef ESP32_DEVKIT_C_V4
#define PIN_RESET_WIFI 0
#endif

#ifdef BOARD_HAS_PSRAM
#define PIN_RESET_WIFI_S3 45
#endif

class PersonalServer : public WiFiManager
{
public:
#ifdef ESP32_DEVKIT_C_V4
    uint8_t pinResetWifi = PIN_RESET_WIFI;
#endif

#ifdef BOARD_HAS_PSRAM
    uint8_t pinResetWifi = PIN_RESET_WIFI_S3;
#endif

    PersonalServer(uint8_t timeout);
    void initWifiManager();
    void saveConfigFile(void);
    int loadConfigFile(void);
    void saveConfigCallback();
    void configModelCallback(WiFiManager *myWiFiManager);
    void checkConfigPortalRequest();

private:
    bool res;
    bool shouldSaveConfig = false;
    char testString[50];
    int testNumber;
    char SSID[50];
    char PASS[50];
};

#endif // PERSONALSERVER_H
