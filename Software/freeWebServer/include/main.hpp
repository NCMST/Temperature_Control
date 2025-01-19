#ifndef MAIN_H
#define MAIN_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <SPIFFS.h>
#include <LittleFS.h>

#include "pid.hpp"
#include "WebServerManager.hpp"
#include "screen.hpp"
#include "temperature.hpp"

// function for FREERTOS

#define SERVER_TASK_STACK_SIZE      8192
#define TEMPERATURE_TASK_STACK_SIZE 2048
#define DISPLAY_TASK_STACK_SIZE     4096
#define PID_TASK_STACK_SIZE         2048

#define BAUD_RATE                   115200
#define LED_BUILTIN                 2
#define MOC_PIN                     23
#define ZCD_PIN                     36 

#define AP_SSID_WORK                "NCMST"
#define AP_PASS_WORK                "N@n0Teh2016"

#define AP_SSID                     "UltraFast"
#define AP_PASS                     "Fastet1234"

#define WF_SSID                     "NCMST_1"
#define WF_PASS                     "password123"

#define LOG_MESSAGE false

void webServerTask(void* pvParameters);
void displayTask(void* pvParameters);
void temperatureTask(void *pvParameters);
void pidTaskHandle(void *pvParameters);


#endif // MAIN_H