#ifndef MAIN_H
#define MAIN_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "pid.hpp"
#include "WebServerManager.hpp"
#include "screen.hpp"
#include "temperature.hpp"

// function for FREERTOS

#define SERVER_TASK_STACK_SIZE 8192
#define TEMPERATURE_TASK_STACK_SIZE 1024
#define DISPLAY_TASK_STACK_SIZE 2048

#define BAUD_RATE 115200



QueueHandle_t temperatureQueue;

void webServerTask(void* pvParameters);
void displayTask(void* pvParameters);
void temperatureTask(void *pvParameters);


#endif // MAIN_H