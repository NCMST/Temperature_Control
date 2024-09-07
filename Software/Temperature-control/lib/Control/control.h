#pragma once

#ifndef CONTROL_H
#define CONTROL_H

#include <SPI.h>
#include <Wire.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "temperature.h"

// pin de control
#define PWM_PIN 23
#define ZD_PIN 36

// PID
#define NUMBER_OF_TEMP_VALUE 200 // on graph
#define KP_VALUE 0.1
#define KI_VALUE 0.05
#define KD_VALUE 0.01
#define PERIOD_VALUE 20
#define MIN_PID_VALUE 0
#define MAX_PID_VALUE 1

void initControl(Temperature dataT);

void compute(Temperature dataT);

#endif // CONTROL_H