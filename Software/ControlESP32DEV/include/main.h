#ifndef MAIN_H
#define MAIN_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "webinterface.hpp"
#include "screen.hpp"
#include "temperature.hpp"

// function for FREERTOS
void displayTask(void *pvParameters);
void displayGraphTask(void *pvParameters);
void PIDControlTask(void *pvParameters);


#endif // MAIN_H