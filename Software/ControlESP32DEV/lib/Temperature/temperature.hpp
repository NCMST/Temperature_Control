#ifndef TEMPERATURE_HPP
#define TEMPERATURE_HPP

#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <SPI.h>
#include <Wire.h>

#include <GyverMAX6675.h>
#include <GyverPID.h>

// MAX6675
#define CLK_PIN 14
#define DATA_PIN 12
#define CS_PIN 15

// Control 
#define PWM_PIN 23
#define NTC_PIN 39
#define ZD_PIN 36
#define PWM_CHANNEL 0        // Canalul PWM
#define PWM_FREQUENCY 5000   // Frecvența PWM în Hz
#define PWM_RESOLUTION 8     // Rezoluția PWM (8 biți)

// NTC parameters
#define NTC_A 0.001129148
#define NTC_B 0.000234125
#define NTC_C 0.0000000876741

#define VCC 3.3
#define NTC_R2 9990
#define ADC_RESOLUTION 4096

// PID configuration
#define NUMBER_OF_TEMP_VALUE 200
#define KP_VALUE 0.1
#define KI_VALUE 0.05
#define KD_VALUE 0.01
#define PERIOD_VALUE 200
#define MIN_PID_VALUE 0
#define MAX_PID_VALUE 1

// Flag index
#define TIME_START 0
#define START_FLAG_INDEX 1

class Temperature 
{
private:
    GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> max6675;
    GyverPID pid;

    float K_Temp;

public:

    Temperature(float new_kp = KP_VALUE, float new_ki = KI_VALUE, float new_kd = KD_VALUE, uint32_t new_dt = PERIOD_VALUE) : pid(new_kp, new_ki, new_kd, new_dt){
    };

    float readKTemp(void);
    double readNTCTemp(void);
    void setKp(float new_kp){pid.Kp = new_kp;};
    void setKi(float new_ki){pid.Ki = new_ki;};
    void setKd(float new_kd){pid.Kd = new_kd;};
    void setDt(uint32_t new_dt){pid.setDt(new_dt);}
    void setSetpoint(float new_setpoint){pid.setpoint = new_setpoint;}
    void setInput(float new_input){pid.input = new_input;}
    void setLimit(uint16_t min_output, uint16_t max_output){pid.setLimits(min_output, max_output);}
    float getKp(void){return pid.Kp;};
    float getKi(void){return pid.Ki;};
    float getKd(void){return pid.Kd;};
    float getInput(void){return pid.input;}
    float getStetpoint(void){return pid.setpoint;}
    float getOutput(void){return pid.output;}
    float getResult(void){return pid.getResult();}
    float getResultNow(void){return pid.getResultNow();}
    float getResultTimer(void){return pid.getResultTimer();}
    void setDirection(bool direction){pid.setDirection(direction);}
    void setMode(bool mode) {pid.setMode(mode);}


    ~Temperature();
};

#endif // TEMPERATURE_HPP