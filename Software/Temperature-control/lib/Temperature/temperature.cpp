#include "temperature.h"

Temperature ::Temperature(void)
    : GyverPID()
{
    names[0] = "Real_temp";
    names[0] = "Set_temp";
    Last_K_Temp = Last_NTC_Temp = Last_set_Temp = NTC_Temp = set_Temp = K_Temp = 0;
    list = String("real, seted, time");
    dates[NUMBER_OF_TEMP_VALUE - 1] = GPunix(2024, 10, 9, 0, 0, 0, 0);
    Kd = Ki = Kp = 0;
}

void Temperature::readKTemp(float *temperature)
{   
    if (sens.readTemp())
    {
        *temperature = sens.getTemp();
    }
}

float Temperature ::readNTCTemp(void)
{
    double Vout, Rth, temperature, adc_value;

    adc_value = ADC_RESOLUTION - analogRead(NTC_PIN) + 0.5; // switch direction
    Vout = (adc_value * VCC) / ADC_RESOLUTION;
    Rth = (VCC * NTC_R2 / Vout) - NTC_R2; // Formula for R2 as Pull-down: Vcc-Rth-R2-GND
    /*  Steinhart-Hart Thermistor Equation:
     *  Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
     */
    temperature = (1 / (NTC_A + (NTC_B * log(Rth)) + (NTC_C * pow((log(Rth)), 3))));
    return temperature - 273.15;
}
