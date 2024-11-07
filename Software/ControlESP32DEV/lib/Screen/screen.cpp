#include "screen.hpp"

/**
 * @brief Initialize ssd1306
 * 
 * @author Catalin  
 * @date 07/11/2024
 * @return true
 * @return false
 * 
 */
bool Screen::init() {
    if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("Error - can't find SSD1306."));
        return false;
    }
    clearDisplay(); 
    display();
    vTaskDelay(2000); 
    return true;
}

/**
 * @brief function for print text on ssd1306
 * 
 * @author Catalin
 * @date 07/11/2024
 * @param x 
 * @param y 
 * @param size 
 * @param text 
 * @return true 
 * @return false 
 */
bool Screen::printText(uint8_t x, uint8_t y, uint8_t size, String text) {

    if(x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT || size < 1 || text.length() == 0)
        return false; //Invalid coordinates, size or text

    setTextColor(SSD1306_WHITE);
    setTextSize(size); 
    setCursor(x,y);
    println(text);
    display();

    return true;
}

bool Screen::clearDisplay() {
    clearDisplay();
    display();
    return true;
}