#pragma once
// for ESP32 NODEMCU
#ifndef CONFIG_H
#define CONFIG_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <GyverMAX6675.h>
#include "GyverPID.h"
#include <GyverPortal.h>

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// pini de indicare
#define LED_PIN 2
#define LCD_SCL 22
#define LCD_SDA 21

// set value of SSID and password
#define AP_SSID "**"
#define AP_PASS "**"

// Pinii modulului MAX6675K
#define CLK_PIN 14
#define DATA_PIN 12
#define CS_PIN 15

// pin de control
#define PWM_PIN 23
#define ZD_PIN 36

#define NTC_PIN 39

#define NTC_A 0.001129148  // thermistor equation parameters
#define NTC_B 0.000234125
#define NTC_C 0.0000000876741

#define VCC 3.3              // NodeMCU on board 3.3v vcc
#define NTC_R2 9970          // 10k ohm series resistor
#define ADC_RESOLUTION 4096  // 12-bit adc

// PID
#define NUMBER_OF_TEMP_VALUE 200  // on graph
#define KP_VALUE 0.1
#define KI_VALUE 0.05
#define KD_VALUE 0.01
#define PERIOD_VALUE 20
#define MIN_PID_VALUE 0
#define MAX_PID_VALUE 1
// FLAGS
#define START_FLAG_INDEX 0
#endif  // CONFIG_H