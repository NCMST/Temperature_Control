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
    // Constructorul
    WebServerManager(const char* ssid, const char* password, const char* second_ssid, const char* second_password);

    int begin();

    void handleHome();

    void handleGraph();

    void handleTemperatureData();

    void handleTemperatureList();

    void handleClient();

    void setTemperatureData(const TemperatureData& tempData);

    void handleSetPoint();

    void handleCommand();

    const TemperatureData& getTemperature() const { return currentTemperature; }

    String readFile(const char* path);

    float getStetTemperature() const { return stetTemperature; }

    bool getStartFlag() const { return startFlag; }

    int getSetTime() const { return setTime; }

    void setWiFiCredentials(const char* ssid, const char* password){
        this->ssid = ssid;
        this->password = password;
    }

    void setupWiFIRouter(const char* ssid, const char* password);

private:
    const char* ssid;         // SSID-ul rețelei Wi-Fi
    const char* password;     // Parola rețelei Wi-Fi
    const char* second_ssid;         // SSID-ul rețelei Wi-Fi
    const char* second_password;     // Parola rețelei Wi-Fi
    WebServer server;         // Obiectul serverului web
    String homePage;          // Pagina principală HTML
    String graphPage;         // Pagina graficului HTML
    String listPage;

    float stetTemperature;
    bool startFlag;
    uint32_t setTime;
    

    std::vector<TemperatureData> temperatureHistory;

    TemperatureData currentTemperature; // Variabila pentru stocarea temperaturii curente

    WiFiMulti wifiMulti; // Obiectul pentru gestionarea mai multor rețele Wi-Fi
};

#endif // WEBSERVERMANAGER_HPP