/**
 * @file screen.hpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/**
 * @brief Screen width 
 * 
 */
#define SCREEN_WIDTH 128 

/**
 * @brief Screen height
 * 
 */
#define SCREEN_HEIGHT 64 

/**
 * @brief Screen reset pin
 * 
 * -1 if the screen does not have a reset pin
 * 
 */
#define OLED_RESET -1   

/**
 * @brief Screen address
 * 
 * I2C Address for the screen
 * 
 * @note 0x3C or 0x3D
 */
#define SCREEN_ADDRESS 0x3C 

/**
 * @class Screen
 * @brief A class to manage the OLED screen using Adafruit_SSD1306 library.
 * 
 * This class provides methods to initialize, clear, and print text on the screen.
 * 
 * @note Inherits from Adafruit_SSD1306.
 * 
 * @fn Screen::Screen(int8_t reset_pin)
 * @brief Construct a new Screen object.
 * 
 * @param reset_pin The pin number used to reset the screen. Default is OLED_RESET.
 * 
 * @fn Screen::~Screen()
 * @brief Destroy the Screen object.
 * 
 * @fn bool Screen::init()
 * @brief Initialize the screen.
 * 
 * @return true if the screen is initialized successfully.
 * @return false if the screen initialization fails.
 * 
 * @fn bool Screen::printText(uint8_t x, uint8_t y, uint8_t size, String text)
 * @brief Print text on the screen at specified coordinates.
 * 
 * @param x The x-coordinate where the text will be printed.
 * @param y The y-coordinate where the text will be printed.
 * @param size The size of the text.
 * @param text The text to be printed.
 * 
 * @return true if the text is printed successfully.
 * @return false if the text printing fails.
 */
class Screen : public Adafruit_SSD1306 {
public:
    /**
     * @brief Construct a new Screen object
     * 
     * @param reset_pin 
     */
    Screen(int8_t reset_pin = OLED_RESET) : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, reset_pin) {}

    /**
     * @brief Destroy the Screen object
     * 
     */
    virtual ~Screen() noexcept {}
    
    /**
     * @brief Initialize the screen
     * 
     * @return true if the screen is initialized
     * @return false if the screen is not initialized
     */
    bool init();

    /**
     * @brief Clear the screen
     * 
     * @return true if the screen is cleared
     * @return false if the screen is not cleared
     * 
     * @param x 
     * @param y
     * @param size
     * @param text
     */
    bool printText(uint8_t x, uint8_t y, uint8_t size, String text);
};

#endif // SCREEN_HPP