#include "temperature.hpp"

/**
 * @brief Read temperature from MAX6675s
 * @date 05/11/2024
 * @author Catalin
 * @return float
 */
float Temperature::readKTemp(void)
{
    if (max6675.readTemp())
    {
        K_Temp = max6675.getTemp();
        if (K_Temp != 0.0f)
        {
            return K_Temp;
        }
    }
    return NAN;
}

/**
 * @brief read temperature from NTC
 * @date 05/11/2024
 * @author Catalin
 * @return double
 */
double Temperature::readNTCTemp(void)
{
    const auto Vout = static_cast<double>(ADC_RESOLUTION - analogRead(NTC_PIN)) * VCC / ADC_RESOLUTION;
    const auto Rth = (VCC * NTC_R2 / Vout) - NTC_R2;

    const auto temperature = (1.0 / (NTC_A + (NTC_B * std::log(Rth)) + (NTC_C * std::pow(std::log(Rth), 3.0))));
    
    if (Vout <= 0)
    {
        Serial.println("Error: Vout is zero or negative.");
        return NAN; 
    }

    if ((temperature - 273.15) > 100 || (temperature - 273.15) < -5)
    {
        return NAN;
    }
    return temperature - 273.15; // Convert in Celsius
}

Temperature::~Temperature()
{
    delete &max6675;
    delete &pid;
}