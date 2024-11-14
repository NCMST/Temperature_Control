#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

class Screen : public Adafruit_SSD1306 {
public:
    Screen(int8_t reset_pin = OLED_RESET) : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, reset_pin) {}
    // free memory
    virtual ~Screen() noexcept {}
    
    bool init();
    bool printText(uint8_t x, uint8_t y, uint8_t size, String text);
    bool clearDisplay();
};

#endif // SCREEN_HPP