#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <GyverPortal.h>
#include "GyverPortal.h"

#include "screen.hpp"
#include "temperature.hpp"
#include "screen.hpp"

// #define AP_SSID "NCMST"
// #define AP_PASS "N@n0Teh2016"

#define AP_SSID "UltraFast"
#define AP_PASS "Fastet123"

#define WF_SSID "ESP32_Config"
#define WF_PASS "password123"

extern GyverPortal ui;
extern Temperature dataT;

extern float set_Temp;
extern int16_t temp[2][NUMBER_OF_TEMP_VALUE];
extern const char* names[2];
extern String list;
extern uint32_t dates[NUMBER_OF_TEMP_VALUE];
extern uint8_t flag[START_FLAG_INDEX + 1];
extern uint32_t startTime, setTime;

void initWifi(void);
void build();
void action();

void startAccessPoint();
void wF_build();
void wF_action();
String formatTime(unsigned long milliseconds);

#endif