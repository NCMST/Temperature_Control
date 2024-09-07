#include "screen.h"

void Screen ::initialize(void)
{
    if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("Eroare - SSD1306 nu a fost găsit."));
        while (1)
            ; // Blochează programul în caz de eroare
    }
    display();      // Afișează ecranul inițial gol
    delay(2000);    // Așteptați 2 secunde
    clearDisplay(); // Ștergeți ecranul pentru a pregăti pentru text
}

void Screen ::printText(uint8_t x, uint8_t y, String text, uint8_t size)
{
    // clearDisplay();  // Ștergeți ecranul pentru a afișa text nou
    setTextColor(SSD1306_WHITE);
    setTextSize(size);
    setCursor(x, y);
    println(text);
    display();
}
