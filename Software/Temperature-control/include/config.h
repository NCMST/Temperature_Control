#pragma once

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

// Definirea dimensiunilor ecranului OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Definirea pinilor de conectare
#define LED_PIN 2
#define LCD_SCL 22
#define LCD_SDA 21

// Setările SSID-ului și parolei WiFi-ului
#define AP_SSID "NCMST"
#define AP_PASS "N@n0Teh2016"

// #define AP_SSID "UltraFast"
// #define AP_PASS "Fastet123"

// Pinii modulului MAX6675K
#define CLK_PIN 14
#define DATA_PIN 12
#define CS_PIN 15

// Pin de control PWM și NTC
#define PWM_PIN 23
#define NTC_PIN 39
#define ZD_PIN 36
#define PWM_CHANNEL 0        // Canalul PWM
#define PWM_FREQUENCY 5000   // Frecvența PWM în Hz
#define PWM_RESOLUTION 8     // Rezoluția PWM (8 biți)

// Parametrii ecuației termistorului NTC
#define NTC_A 0.001129148
#define NTC_B 0.000234125
#define NTC_C 0.0000000876741

#define VCC 3.3
#define NTC_R2 9990
#define ADC_RESOLUTION 4096

// Configurarea PID
#define NUMBER_OF_TEMP_VALUE 200
#define KP_VALUE 0.1
#define KI_VALUE 0.05
#define KD_VALUE 0.01
#define PERIOD_VALUE 200
#define MIN_PID_VALUE 0
#define MAX_PID_VALUE 1

// Indici pentru flag-uri
#define TIME_START 0
#define START_FLAG_INDEX 1


#endif // CONFIG_H