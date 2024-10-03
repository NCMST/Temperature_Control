#include "data.h"
#include <GyverMAX6675.h>

GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> sens;

float Data::readKTemp() {
    if (sens.readTemp()) { // Verifică dacă citirea a avut succes
        K_Temp = sens.getTemp(); // Obține temperatura
        if (K_Temp != 0) { // Verifică dacă temperatura nu este 0
            return K_Temp; // Returnează temperatura validă
        }
    }
    return NAN; // Returnează NaN dacă citirea eșuează sau temperatura este 0
}

float Data::readNTCTemp()
{
    double Vout = (ADC_RESOLUTION - analogRead(NTC_PIN)) * VCC / ADC_RESOLUTION;
    double Rth = (VCC * NTC_R2 / Vout) - NTC_R2;

    double temperature = (1 / (NTC_A + (NTC_B * log(Rth)) + (NTC_C * pow(log(Rth), 3))));
    return temperature - 273.15; // Convertire în grade Celsius
}