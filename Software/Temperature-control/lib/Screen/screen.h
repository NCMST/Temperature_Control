#pragma once

#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <GyverMAX6675.h>
#include "GyverPID.h"
#include <GyverPortal.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// LED and SCREEN PINS
#define LED_PIN 2
#define LCD_SCL 22
#define LCD_SDA 21

class Screen : public Adafruit_SSD1306
{

private:
public:
  /***
   * @param reset_pin for screen
   */
  Screen(int8_t reset_pin = OLED_RESET)
      : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, reset_pin) {}

  void initialize(void);

  /***
   * @param x axe point
   * @param y axe poin
   * @param text what xou want to print
   * @param size of text size default 1
   */
  void printText(uint8_t x, uint8_t y, String text, uint8_t size = 1);
};

#endif // SCREEN_H