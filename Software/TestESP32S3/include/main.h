#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <WebServer.h>
#include <EEPROM.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ESP32WiFiConfig.h"
#include <esp_wifi.h>
#include <tcpip_adapter.h>
#include <WiFi.h>

#define RGB_PIN 48  // RGB LED PIN

const char* ap_ssid = "ESP32_Config";
const char* ap_password = "password123";
const int buttonPin = 45;


#endif // MAIN_H