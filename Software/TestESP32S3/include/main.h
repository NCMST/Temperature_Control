#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ESP32WiFiConfig.h"

const char* ap_ssid = "ESP32_Config";
const char* ap_password = "password123";
const int buttonPin = 45;

#endif // MAIN_H