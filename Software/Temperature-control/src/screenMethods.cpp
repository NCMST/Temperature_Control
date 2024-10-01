#include "screenMethods.h"

void Screen::initialize() {
    if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("Eroare - SSD1306 nu a fost găsit."));
        while (true); // Blochează programul în caz de eroare
    }
    clearDisplay(); // Șterge ecranul pentru a pregăti pentru text
    display();
    delay(2000); // Așteptați 2 secunde
}

void Screen::printText(uint8_t x, uint8_t y, uint8_t size, String text) {
    setTextColor(SSD1306_WHITE);
    setTextSize(size); // Setează dimensiunea textului
    setCursor(x,y);
    println(text);
    display();
}