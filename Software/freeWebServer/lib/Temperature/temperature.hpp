/**
 * @file temperature.hpp
 * @author 
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef TEMPERATURE_HPP
#define TEMPERATURE_HPP

#ifdef UNIT_TEST
// Dacă suntem în test, nu include Arduino.h
#include <stdint.h>
#include <math.h>
#include <stddef.h>
#else
#include <Arduino.h>
#include <max6675.h>
#endif

/**
 * @brief PWM_PIN - pin for PWM signal
 * 
 */
#define PWM_PIN 23

/**
 * @brief NTC_PIN - pin for NTC signal
 * 
 */
#define NTC_PIN 39

/**
 * @brief NTC_A - NTC Steinhart-Hart coefficients
 * 
 */
#define NTC_A 0.001129148

/**
 * @brief NTC_B - NTC Steinhart-Hart coefficients
 * 
 */
#define NTC_B 0.000234125

/**
 * @brief NTC_C - NTC Steinhart-Hart coefficients
 * 
 */
#define NTC_C 0.0000000876741

/**
 * @brief NTC_R0 - NTC resistance at 25 degrees Celsius
 * 
 */
#define VCC 3.3

/**
 * @brief NTC_R0 - NTC resistance at 25 degrees Celsius
 * 
 */
#define NTC_R2 9990

/**
 * @brief ADC_RESOLUTION - ADC resolution
 * 
 */
#define ADC_RESOLUTION 4096

/**
 * @brief CS_PIN - Chip Select pin 
 * 
 */
#define CS_PIN 15

/**
 * @brief CLK_PIN - Clock pin
 * 
 */
#define CLK_PIN 14

/**
 * @brief DATA_PIN - Data pin
 * 
 */
#define DATA_PIN 12

class Temperature 

/**
 * @class Temperature
 * 
 * @brief A class to handle temperature readings from a K type thermocouple and an NTC thermistor.
 * 
 * This class provides methods to read temperatures from a K type thermocouple using the Adafruit_MAX6675 sensor
 * and from an NTC thermistor. It abstracts the hardware-specific details and provides a simple interface
 * for obtaining temperature readings.
 * 
 * @note Ensure that the CLK_PIN, DATA_PIN, and CS_PIN are correctly defined for the Adafruit_MAX6675 sensor.
 * 
 * @attention This class assumes that the necessary hardware connections are made and the Adafruit_MAX6675 library is included.
 * 
 * @warning The temperature readings are dependent on the accuracy of the sensors and the correct configuration of the pins.
 * 
 * @see Adafruit_MAX6675
 * 
 * @fn float readKTemp(void)
 * @fn double readNTCTemp(void)
 * 
 */

{
private:
    MAX6675  max6675;   
public:
    /**
     * @brief Construct a new Temperature object
     * 
     */
    Temperature() : max6675(CLK_PIN, CS_PIN, DATA_PIN) {}

    /**
     * @brief readKTemp - read K type thermocouple temperature
     * 
     * the temperature of the thermocouple (heater) 
     * 
     * @return float 
     */
    float readKTemp(void);

    /**
     * @brief readNTCTemp - read NTC temperature
     * 
     * the room temperature
     * 
     * @return double 
     */
    double readNTCTemp(void);

    /**
     * @brief Destroy the Temperature object
     * 
     */
    ~Temperature();
};

#endif // TEMPERATURE_HPP