#include "pid.hpp"

PID::PID(float kp, float ki, float kd) : kp(kp), ki(ki), kd(kd), integral(0), last_error(0) {}

int PID::compute(float setpoint, float measured_value) {
    float error = setpoint - measured_value;
    integral += error;

    // Calculate the derivative
    float derivative = error - last_error;
    last_error = error;

    // Calculate the PID output
    float output = kp * error + ki * integral + kd * derivative;

    // Limit the output between output_min and output_max
    if (output > output_max) {
        output = output_max;
        integral -= error; // Anti-windup: adjust the integral if the output is limited
    } else if (output < output_min) {
        output = output_min;
        integral -= error; // Anti-windup: adjust the integral if the output is limited
    }

    return output;
}

void PID::setLimits(float min, float max) {
    output_min = min;
    output_max = max;
}
