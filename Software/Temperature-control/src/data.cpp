#include "data.h"
#include <GyverMAX6675.h>

GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> sens;

float Data::readKTemp() {
    if (sens.readTemp()) {
        K_Temp = sens.getTemp();
        Serial.print("Temperatura citită: ");
        Serial.println(K_Temp); // Afișează temperatura citită
        return K_Temp;
    }
    Serial.println("Eroare la citirea temperaturii!"); // Mesaj de eroare
    return NAN;
}

float Data::readNTCTemp() {
    double Vout = (ADC_RESOLUTION - analogRead(NTC_PIN)) * VCC / ADC_RESOLUTION;
    double Rth = (VCC * NTC_R2 / Vout) - NTC_R2;

    double temperature = (1 / (NTC_A + (NTC_B * log(Rth)) + (NTC_C * pow(log(Rth), 3))));
    return temperature - 273.15; // Convertire în grade Celsius
}