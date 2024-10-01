#pragma once

#include <GyverPID.h>
#include "config.h"

class Data : public GyverPID {
public:
    float K_Temp, NTC_Temp, set_Temp;
    int16_t temp[2][NUMBER_OF_TEMP_VALUE];
    const char* names[2];
    String list;
    uint32_t dates[NUMBER_OF_TEMP_VALUE];
    uint8_t flag[START_FLAG_INDEX + 1];

    Data() : GyverPID() {
        names[0] = "Real_temp";
        names[1] = "Set_temp";
        K_Temp = NTC_Temp = set_Temp = 0;
        list = String("real,seted,time");
        dates[NUMBER_OF_TEMP_VALUE - 1] = GPunix(2023, 10, 9, 0, 0, 0);
        Kd = Ki = Kp = 0;
    }

    float readKTemp();
    float readNTCTemp();
};

extern Data dataT;