#include "control.h"

void initControl(Temperature dataT)
{
    dataT.Kp = KP_VALUE;
    dataT.Ki = KI_VALUE;
    dataT.Kd = KD_VALUE;
    dataT.setMode(ON_ERROR);
    dataT.setDirection(NORMAL);
    dataT.setLimits(MIN_PID_VALUE, MAX_PID_VALUE);
    dataT.setDt(PERIOD_VALUE);
}

void compute(Temperature dataT)
{
    // if is started
    if (dataT.flag[START_FLAG_INDEX])
    {
        float ktemp;
        dataT.readKTemp(&ktemp);

        dataT.input = ktemp;
        digitalWrite(PWM_PIN, dataT.getResult());
    }
}