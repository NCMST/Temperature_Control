#ifndef TEMPERATURE_HPP
#define TEMPERATURE_HPP

#ifdef UNIT_TEST
// Dacă suntem în test, nu include Arduino.h
#include <stdint.h>
#include <math.h>
#include <stddef.h>
#else
#include <Arduino.h>
#include <GyverMAX6675.h>
#endif

// Control 
#define PWM_PIN 23
#define NTC_PIN 39

// NTC parameters
#define NTC_A 0.001129148
#define NTC_B 0.000234125
#define NTC_C 0.0000000876741

#define VCC 3.3
#define NTC_R2 9990
#define ADC_RESOLUTION 4096

#define CS_PIN 15
#define CLK_PIN 14
#define DATA_PIN 12

class Temperature 
{
private:
    GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> max6675;
public:
    Temperature() {}

    float readKTemp(void);
    double readNTCTemp(void);

    ~Temperature();
};

#endif // TEMPERATURE_HPP