#include "temperature.hpp"

float Temperature::readKTemp(void) {
    if (max6675.readTemp()) { 
        return max6675.getTemp(); 
    }
    return false; 
}

double Temperature::readNTCTemp(void) {
    const auto Vout = static_cast<double>(ADC_RESOLUTION - analogRead(NTC_PIN)) * VCC / ADC_RESOLUTION;
    const auto Rth = (VCC * NTC_R2 / Vout) - NTC_R2;

    const auto temperature = (1.0 / (NTC_A + (NTC_B * std::log(Rth)) + (NTC_C * std::pow(std::log(Rth), 3.0))));
    return temperature - 273.15; // Convert in Celsius
}

Temperature::~Temperature() {}