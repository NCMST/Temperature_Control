/**
 * @file temperature.cpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "temperature.hpp"

/**
 * @brief Read the temperature of the K type thermocouple
 * 
 * temperature of the heater
 * 
 * @return float 
 */
float Temperature::readKTemp(void) {
    float temp = max6675.readCelsius();
    if (temp != NAN) { 
        return temp; 
    }
    return false; 
}

/**
 * @brief Read the temperature of the NTC thermistor
 * 
 * Temperature of the room
 * 
 * @return double 
 */
double Temperature::readNTCTemp(void) {
    const auto Vout = static_cast<double>(ADC_RESOLUTION - analogRead(NTC_PIN)) * VCC / ADC_RESOLUTION;
    const auto Rth = (VCC * NTC_R2 / Vout) - NTC_R2;

    const auto temperature = (1.0 / (NTC_A + (NTC_B * std::log(Rth)) + (NTC_C * std::pow(std::log(Rth), 3.0))));
    return temperature - 273.15; // Convert in Celsius
}

/**
 * @brief Destroy the Temperature object
 * 
 */
Temperature::~Temperature() {}