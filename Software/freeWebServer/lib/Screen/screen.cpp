#include "screen.hpp"

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

bool Screen::printText(uint8_t x, uint8_t y, uint8_t size, String text) {
    setTextColor(SSD1306_WHITE);
    setTextSize(size); 
    setCursor(x,y);
    println(text);
    display();
    return true;
}