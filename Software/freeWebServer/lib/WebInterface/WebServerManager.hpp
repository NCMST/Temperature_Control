#ifndef WEBSERVERMANAGER_HPP
#define WEBSERVERMANAGER_HPP

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <vector>
#include <FS.h>
#include <SPIFFS.h>
#include "TemperatureData.hpp"

#define HTTP_PORT 80
#define LOGS_MESSAGE false

// #define AP_SSID "NCMST"
// #define AP_PASS "N@n0Teh2016"

#define AP_SSID "UltraFast"
#define AP_PASS "Fastet123"

#define WF_SSID "ESP32_Config"
#define WF_PASS "password123"


class WebServerManager {
public:
    // Constructorul
    WebServerManager(const char* ssid, const char* password);

    // Funcția de inițializare a serverului
    void begin();

    // Funcția de gestionare a cererilor pentru pagina principală
    void handleHome();

    // Funcția de gestionare a cererilor pentru pagina graficului
    void handleGraph();

    // Funcția de gestionare a cererilor pentru datele temperaturii
    void handleTemperatureData();

    void handleTemperatureList();

    // Funcția de gestionare a cererilor clientului
    void handleClient();

    // Funcția pentru a primi temperatura dintr-un task
    void setTemperatureData(const TemperatureData& tempData);

    void handleSetPoint();

    void handleCommand();

    const TemperatureData& getTemperature() const { return currentTemperature; }

    String readFile(const char* path);

    float getStetTemperature() const { return stetTemperature; }

    bool getStartFlag() const { return currentTemperature.startFlag; }

    int getSetTime() const { return currentTemperature.setTime; }

private:
    const char* ssid;         // SSID-ul rețelei Wi-Fi
    const char* password;     // Parola rețelei Wi-Fi
    WebServer server;         // Obiectul serverului web
    String homePage;          // Pagina principală HTML
    String graphPage;         // Pagina graficului HTML
    String listPage;

    float stetTemperature;
    

    std::vector<TemperatureData> temperatureHistory;

    TemperatureData currentTemperature; // Variabila pentru stocarea temperaturii curente
};

#endif // WEBSERVERMANAGER_HPP