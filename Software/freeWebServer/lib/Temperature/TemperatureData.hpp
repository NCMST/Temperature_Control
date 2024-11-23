#ifndef TEMPERATUREDATA_HPP
#define TEMPERATUREDATA_HPP

struct TemperatureData {
    
    String timestamp;        // Timpul la care a fost înregistrată temperatura

    float inside_temperature; // in °C
    float outside_temperature; // in °C
    float setpoint_temperature;
};

#endif