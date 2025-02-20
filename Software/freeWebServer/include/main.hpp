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
#define PID_TASK_STACK_SIZE         4096

#define BAUD_RATE                   115200
#define LED_BUILTIN                 2
#define MOC_PIN                     23
#define ZCD_PIN                     36 

#ifdef USE_CONFIG_H
#include "config.h"
#else
#define AP_SSID_WORK "your_default_ssid_work"
#define AP_PASS_WORK "your_default_pass_work"

#define AP_SSID "your_default_ssid"
#define AP_PASS "your_default_pass"

#define WF_SSID "your_default_ssid"
#define WF_PASS "your_default_pass"
#endif

#define LOG_MESSAGE false
#define LOGS_OFFSET pdMS_TO_TICKS(10000)

#define SECOND pdMS_TO_TICKS(1000)
#define MINUT pdMS_TO_TICKS(1000 * 60)

void webServerTask(void* pvParameters);
void displayTask(void* pvParameters);
void temperatureTask(void *pvParameters);
void pidTaskHandle(void *pvParameters);

#endif // MAIN_H