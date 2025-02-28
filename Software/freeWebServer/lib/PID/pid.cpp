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
int PID::compute(float setpoint, float measured_value)
{
    // Apply a simple low-pass filter to the measured value
    static float filtered_input = measured_value;
    const float alpha = (abs(measured_value - filtered_input) > 5.0) ? 0.7 : 0.3;
    filtered_input = alpha * filtered_input + (1 - alpha) * measured_value;

    float error = setpoint - filtered_input;
    integral += error;

    // Calculate the derivative
    float derivative = error - last_error;
    last_error = error;

    // Calculate the PID output
    float output = kp * error + ki * integral + kd * derivative;

    // Limit the output between output_min and output_max
    if (output > output_max)
    {
        integral -= error * (1.0 + 0.1 * abs(error)); // Corecție neliniară
    }
    if (abs(error) < 0.5)
    {                     // Zonă moartă de ±0.5°C
        integral *= 0.99; // Prevenire drift
        derivative = 0;
    }

    return output;
}

/**
 * @brief Set the output limits for the PID controller.
 *
 * @param min The minimum output value.
 * @param max The maximum output value.
 */
void PID::setLimits(float min, float max)
{
    output_min = min;
    output_max = max;
}
