// lib/ESP32WiFiConfig/ESP32WiFiConfig.h

#ifndef ESP32WIFICONFIG_H
#define ESP32WIFICONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <esp_wifi.h>
#include <tcpip_adapter.h>

class ESP32WiFiConfig {
public:
    ESP32WiFiConfig(const char* ap_ssid, const char* ap_password, int buttonPin, WebServer& server);
    void setup();
    void loop();
    bool checkWiFiConnection();
    void startAP();
    void checkConnectedDevices();
    bool isConnected();

private:
    const char* ap_ssid;
    const char* ap_password;
    int buttonPin;
    bool shouldStartAP;
    WebServer& server;

    
    void handleRoot();
    void handleSave();
    String readEEPROM(int startAddr);
    void writeEEPROM(int startAddr, String data);
    bool connected;

};

#endif // ESP32WIFICONFIG_H