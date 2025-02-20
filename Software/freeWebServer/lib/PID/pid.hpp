/**
 * @file pid.hpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef PID_H
#define PID_H

class PID
/**
 * @class PID
 * @brief A class to implement a PID controller.
 * 
 * The PID class provides methods to set PID coefficients, compute the output value,
 * and set limits for the output value. It also provides getter and setter methods
 * for the PID coefficients.
 * 
 * @note The compute method should be called periodically to update the output value.
 * 
 * @var PID::kp
 * Proportional gain.
 * 
 * @var PID::ki
 * Integral gain.
 * 
 * @var PID::kd
 * Derivative gain.
 * 
 * @var PID::integral
 * Integral term.
 * 
 * @var PID::last_error
 * Last error value.
 * 
 * @var PID::output_min
 * Minimum limit for the output value.
 * 
 * @var PID::output_max
 * Maximum limit for the output value.
 */
{
public:
    /**
     * @brief Construct a new PID object
     * 
     * @param kp 
     * @param ki 
     * @param kd 
     */
    PID(float kp, float ki, float kd);

    /**
     * @brief Set the limits for the output value
     * 
     * @param min 
     * @param max 
     */
    void setLimits(float min, float max);

    /**
     * @brief Compute the output value
     * 
     * @param setpoint 
     * @param measured_value 
     * 
     * @return float 
     */
    int compute(float setpoint, float measured_value);

    /**
     * @brief Set the Kp object
     * 
     * @param new_kp 
     */
    void setKp(float new_kp) { kp = new_kp; }

    /**
     * @brief Set the Ki object
     * 
     * @param new_ki 
     */
    void setKi(float new_ki) { ki = new_ki; }

    /**
     * @brief Set the Kd object
     * 
     * @param new_kd 
     */
    void setKd(float new_kd) { kd = new_kd; }

    /**
     * @brief Get the Kp object
     * 
     * @return float 
     */
    float getKp(void) { return kp; }

    /**
     * @brief Get the Ki object
     * 
     * @return float 
     */
    float getKi(void) { return ki; }

    /**
     * @brief Get the Kd object
     * 
     * @return float 
     */
    float getKd(void) { return kd; }

private:
    float kp; // proportional gain
    float ki; // integral gain
    float kd; // derivative gain
    float integral; // integral term
    float last_error; // last error
    float output_min; // output min limit
    float output_max; // output max limit
};

#endif