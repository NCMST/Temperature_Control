#ifndef ESP32WIFICONFIG_H
#define ESP32WIFICONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

class ESP32WiFiConfig {
public:
    ESP32WiFiConfig(const char* ap_ssid, const char* ap_password, int buttonPin, WebServer& server);
    void setup();
    void loop();

private:
    const char* ap_ssid;
    const char* ap_password;
    int buttonPin;
    bool shouldStartAP;
    WebServer& server;

    void startAP();
    void handleRoot();
    void handleSave();
    String readEEPROM(int startAddr);
    void writeEEPROM(int startAddr, String data);
};

#endif // ESP32WIFICONFIG_H