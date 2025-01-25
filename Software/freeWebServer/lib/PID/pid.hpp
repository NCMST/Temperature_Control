#ifndef PID_H
#define PID_H

/*PID algorithm class by Catalin*/

class PID
{
public:
    PID(float kp, float ki, float kd);
    void setLimits(float min, float max);
    int compute(float setpoint, float measured_value);

    void setKp(float new_kp) { kp = new_kp; }
    void setKi(float new_ki) { ki = new_ki; }
    void setKd(float new_kd) { kd = new_kd; }

    float getKp(void) { return kp; }
    float getKi(void) { return ki; }
    float getKd(void) { return kd; }

private:
    float kp;
    float ki;
    float kd;
    float integral;
    float last_error;
    float output_min;
    float output_max;
};

#endif