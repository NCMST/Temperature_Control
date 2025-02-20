/**
 * @file screen.cpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "screen.hpp"

/**
 * @brief initialize the screen
 * 
 * @return true 
 * @return false 
 */
bool Screen::init() {
    if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("Error - can't find SSD1306."));
        while (true); 
    }
    clearDisplay(); 
    display();
    vTaskDelay(2000); 
    return true;
}

/**
 * @brief print text on the screen
 * 
 * @param x 
 * @param y 
 * @param size 
 * @param text 
 * @return true 
 * @return false 
 */
bool Screen::printText(uint8_t x, uint8_t y, uint8_t size, String text) {
    setTextColor(SSD1306_WHITE);
    setTextSize(size); 
    setCursor(x,y);
    println(text);
    display();
    return true;
}