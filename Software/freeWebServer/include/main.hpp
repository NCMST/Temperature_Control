/**
 * @file main.hpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MAIN_H
#define MAIN_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Arduino.h>

#include <map>
#include <string>

#include <PID_AutoTune_v0.h>

#include "pid.hpp"
#include "WebServerManager.hpp"
#include "screen.hpp"
#include "temperature.hpp"

// WiFi parameters
#ifdef USE_CONFIG_H
#include "config.h"
#else
std::map<std::string, std::string> wifiConfig = {
    {"NCMST", "Tech0dev2016"},
    {"UltraFast", "Fastet123"},
    {"NCMST", "password"}
};
#endif

#define WIFI_PASS_NR 2

// Task stack sizes
#define SERVER_TASK_STACK_SIZE      8192
#define TEMPERATURE_TASK_STACK_SIZE 2048
#define DISPLAY_TASK_STACK_SIZE     4096
#define PID_TASK_STACK_SIZE         4096

// System parameters
#define BAUD_RATE                   115200
#define LED_BUILTIN                 2
#define MOC_PIN                     23
#define ZCD_PIN                     36 

// Log and time parameters
#define LOG_MESSAGE false
#define LOGS_OFFSET pdMS_TO_TICKS(10000)

#define SECOND pdMS_TO_TICKS(1000)
#define MINUT pdMS_TO_TICKS(1000 * 60)



/**
 * @brief Web server task definition
 * 
 * @param pvParameters 
 */
void webServerTask(void* pvParameters);

/**
 * @brief Display task definition
 * 
 * @param pvParameters 
 */
void displayTask(void* pvParameters);

/**
 * @brief Temperature task definition
 * 
 * @param pvParameters 
 */
/**
 * @brief Task function to handle temperature control.
 *
 * This function is designed to be run as a FreeRTOS task. It handles the 
 * temperature control logic, including reading temperature sensors, 
 * processing the data, and controlling actuators to maintain the desired 
 * temperature.
 *
 * @param pvParameters Pointer to the parameters passed to the task. This 
 *                     can be used to pass configuration or state information 
 *                     to the task.
 */
void temperatureTask(void *pvParameters);

/**
 * @brief PID task definition
 * 
 * @param pvParameters 
 */
void pidTaskHandle(void *pvParameters);



#endif // MAIN_H
