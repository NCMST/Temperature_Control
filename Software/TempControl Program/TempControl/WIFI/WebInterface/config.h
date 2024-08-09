#pragma once
// for ESP32 DEVKIT V1
#ifndef CONFIG_H
#define CONFIG_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Wire.h>
#include <GyverMAX6675.h>
#include "GyverPID.h"
#include <GyverPortal.h>
// #include "function.h"

// set value of SSID and password
#define AP_SSID "**"
#define AP_PASS "**"

// Pinii modulului MAX6675K
#define CLK_PIN 14
#define DATA_PIN 12
#define CS_PIN 15

//LCD PIN
#define SDA_PIN 21
#define SCL_PIN 22
#define LCD_ADDRESS 0x27
#define LCD_COL 16
#define LCD_ROW 2

// pin de control
#define PWM_PIN 16
#define ZD_PIN 23
// pini de indicare
#define GREEN_LED_PIN 18  // setet temperature is now
#define RED_LED_PIN 19    // work but temperature noi is like setet

#define LED_PIN 2

// time interval
#define interval 5000 * 1000
#define interval2 1000 * 1000

#define NUMBER_OF_TEMP_VALUE 200


#endif  // CONFIG_H
