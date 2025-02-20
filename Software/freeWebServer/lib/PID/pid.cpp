/**
 * @file pid.cpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "pid.hpp"

/**
 * @brief Construct a new PID object with specified gains.
 * 
 * @param kp Proportional gain.
 * @param ki Integral gain.
 * @param kd Derivative gain.
 */
PID::PID(float kp, float ki, float kd) : kp(kp), ki(ki), kd(kd), integral(0), last_error(0) {}

/**
 * @brief Compute the PID output based on the setpoint and measured value.
 * 
 * @param setpoint The desired target value.
 * @param measured_value The current measured value.
 * @return int The computed PID output.
 */
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

/**
 * @brief Set the output limits for the PID controller.
 * 
 * @param min The minimum output value.
 * @param max The maximum output value.
 */
void PID::setLimits(float min, float max) {
    output_min = min;
    output_max = max;
}
