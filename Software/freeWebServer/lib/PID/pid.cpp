#include "pid.hpp"

PID::PID(float kp, float ki, float kd) : kp(kp), ki(ki), kd(kd), integral(0), last_error(0) {}

int PID::compute(float setpoint, float measured_value) {
    float error = setpoint - measured_value;
    integral += error;

    // Calcularea derivatei
    float derivative = error - last_error;
    last_error = error;

    // Calcularea ieșirii PID
    float output = kp * error + ki * integral + kd * derivative;

    // Limitarea ieșirii între output_min și output_max
    if (output > output_max) {
        output = output_max;
        integral -= error; // Anti-windup: ajustează integralul dacă ieșirea este limitată
    } else if (output < output_min) {
        output = output_min;
        integral -= error; // Anti-windup: ajustează integralul dacă ieșirea este limitată
    }

    return output;
}

void PID::setLimits(float min, float max) {
    output_min = min;
    output_max = max;
}
