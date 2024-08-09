#pragma once
// for ESP32 NODEMCU
#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <SPI.h>
#include <Wire.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <GyverMAX6675.h>
#include "GyverPID.h"
#include <GyverPortal.h>

// Pinii modulului MAX6675K
#ifdef ESP32_DEVKIT_C_V4
#define CLK_PIN 18
#define DATA_PIN 19 //MISO
#define CS_PIN 5
#endif

#ifdef BOARD_HAS_PSRAM
#define CLK_PIN 30
#define DATA_PIN 31
#define CS_PIN 29
#endif

#define NTC_PIN 39

#define NTC_A 0.001129148 // thermistor equation parameters
#define NTC_B 0.000234125
#define NTC_C 0.0000000876741

#define VCC 3.3             // NodeMCU on board 3.3v vcc
#define NTC_R2 9970         // 10k ohm series resistor
#define ADC_RESOLUTION 4096 // 12-bit adc


#define NUMBER_OF_TEMP_VALUE 200 // on graph

// PID
#define KP_VALUE 0.1
#define KI_VALUE 0.05
#define KD_VALUE 0.01
#define PERIOD_VALUE 20
#define MIN_PID_VALUE 0
#define MAX_PID_VALUE 1


// FLAGS
#define START_FLAG_INDEX 0

class Temperature : public GyverPID
{
private:
    GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> sens;

public:
    float K_Temp, NTC_Temp, set_Temp, Last_K_Temp, Last_NTC_Temp, Last_set_Temp;
    int16_t temp[2][NUMBER_OF_TEMP_VALUE];
    const char *names[2];
    String list;
    uint32_t dates[NUMBER_OF_TEMP_VALUE];
    uint8_t flag[START_FLAG_INDEX + 1];


    Temperature(void);

    /**
     * @brief Reading temperature from MAX6675 module via SPI
     */
    void readKTemp(float *temperature);

    /**
     * @brief Reading temperature from NTC resistor
     */
    float readNTCTemp(void);
};

#endif // TEMPERATURE_H