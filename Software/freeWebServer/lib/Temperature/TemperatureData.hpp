/**
 * @file TemperatureData.hpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef TEMPERATUREDATA_HPP
#define TEMPERATUREDATA_HPP

/**
 * @brief A data structure that holds the temperature data
 * 
 */
struct TemperatureData {

    /**
     * @brief timestamp when the temperature was recorded
     * 
     * in format "YYYY-MM-DD HH:MM:SS"
     * 
     */
    String timestamp;      

    /**
     * @brief the temperature inside the house
     * 
     * in °C
     * 
     */
    float inside_temperature;
    
    /**
     * @brief the temperature outside the house
     * 
     * in °C
     * 
     */
    float outside_temperature;
    
    /**
     * @brief the temperature setpoint
     * 
     * in °C
     * 
     */
    float setpoint_temperature;

    /**
     * @brief the humidity inside the house
     * 
     * in minutes
     * 
     */
    uint32_t setTime;

    /**
     * @brief the humidity outside the house
     * 
     * in minutes
     * 
     */
    uint32_t realTime;
    
    /**
     * @brief the humidity setpoint
     * 
     * as boolean
     * 
     */
    bool startFlag;
};

#endif