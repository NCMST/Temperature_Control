#ifndef WEBSERVERMANAGER_HPP
#define WEBSERVERMANAGER_HPP

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <vector>
#include <FS.h>
#include <SPIFFS.h>
#include "TemperatureData.hpp"

#define HTTP_PORT 80
#define LOGS_MESSAGE true

#define TEN_SEC 5000
#define LED_BUILTIN 2

class WebServerManager {
public:
    // Constructor
    WebServerManager(const char* ssid, const char* password, const char* second_ssid, const char* second_password);

    int begin();

    void handleHome();

    void handleList();

    void handleGraph();

    void handleTemperatureData();

    void handleTemperatureList();

    void handleClient();

    void setTemperatureData(const TemperatureData& tempData);

    void handleSetPoint();

    void handleCommand();

    const TemperatureData& getTemperature() const { return currentTemperature; }

    float getStetTemperature() const { return stetTemperature; }

    bool getStartFlag() const { return startFlag; }

    int getSetTime() const { return setTime; }

    void setWiFiCredentials(const char* ssid, const char* password){
        this->ssid = ssid;
        this->password = password;
    }

    void setupWiFIRouter(const char* ssid, const char* password);

    void handleDownload();

    void updateCSV(float realTemperature, float setTemperature, uint32_t time);

private:
    const char* ssid;         // SSID of the Wi-Fi network
    const char* password;     // Password of the Wi-Fi network
    const char* second_ssid;  // SSID of the second Wi-Fi network
    const char* second_password; // Password of the second Wi-Fi network
    WebServer server;         // Web server object
    String homePage;          // HTML home page
    String graphPage;         // HTML graph page
    String listPage;

    float stetTemperature;
    bool startFlag;
    uint32_t setTime;
    
    std::vector<TemperatureData> temperatureHistory;

    TemperatureData currentTemperature; // Variable for storing the current temperature

    WiFiMulti wifiMulti; // Object for managing multiple Wi-Fi networks
};

#endif // WEBSERVERMANAGER_HPP
