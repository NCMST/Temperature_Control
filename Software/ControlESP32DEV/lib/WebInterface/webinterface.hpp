#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <GyverPortal.h>
#include "GyverPortal.h"

#include "screen.hpp"
#include "temperature.hpp"
#include "screen.hpp"

// #define AP_SSID "NCMST"
// #define AP_PASS "N@n0Teh2016"

#define AP_SSID "UltraFast"
#define AP_PASS "Fastet123"

class WebInterface : public GyverPortal
{
private:
    Temperature dataT;
    Screen screen;

public:
    uint8_t flag[START_FLAG_INDEX + 1];
    int16_t temp[2][NUMBER_OF_TEMP_VALUE];
    uint32_t dates[NUMBER_OF_TEMP_VALUE];
    const char *names[2];
    String list;

    float set_Temp;
    uint32_t startTime, setTime;

    GyverPortal ui;

    WebInterface(void);
    ~WebInterface(void);

    void startWebServer();

    void initWifi(void);
    void build();
    void action();

    bool tick() { return ui.tick(); }

    String formatTime(unsigned long milliseconds);
};

#endif