#pragma once

#include <Adafruit_SSD1306.h>
#include "config.h"

class Screen : public Adafruit_SSD1306 {
public:
    Screen(int8_t reset_pin = OLED_RESET) : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, reset_pin) {}
    
    void initialize();
    void printText(uint8_t x, uint8_t y, uint8_t size, String text);
};