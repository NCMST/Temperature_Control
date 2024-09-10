#include "main.h"

// Adding tasck for Internet and Peripheral
void InternetTask(void *pvParameters);
void PeripheralTask(void *pvParameters);

WebServer server(80);
ESP32WiFiConfig wifiConfig(ap_ssid, ap_password, buttonPin, server);

void setup()
{
    Serial.begin(115200);

    pinMode(RGB_PIN, OUTPUT);

    // Configurare Wi-Fi
    if (!wifiConfig.checkWiFiConnection())
    {
        wifiConfig.startAP();
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
        neopixelWrite(RGB_PIN, 0, 0, 255); // Albastru pentru modul AP

        // Așteptați până când se realizează conexiunea
        while (!wifiConfig.isConnected())
        {
            delay(1000);
            Serial.println("Așteptare conexiune Wi-Fi...");
        }
    }

    Serial.println("Conectat la Wi-Fi");
    Serial.print("Adresa IP: ");
    Serial.println(WiFi.localIP());
    neopixelWrite(RGB_PIN, 0, 255, 0); // Verde pentru conexiune reușită

    // Creează task-ul pentru operațiuni internet pe Core 0
    xTaskCreatePinnedToCore(InternetTask, "InternetTask", 4096, &wifiConfig, 1, NULL, 0);

    // Creează task-ul pentru operațiuni periferice pe Core 1
    xTaskCreatePinnedToCore(PeripheralTask, "PeripheralTask", 4096, NULL, 1, NULL, 1);
}

void loop()
{
    // Loop-ul principal rămâne gol, deoarece task-urile se ocupă de funcționalități
}

void InternetTask(void *pvParameters)
{
    for (;;)
    {
        // Codul pentru Task1
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void PeripheralTask(void *pvParameters)
{
    for (;;)
    {
        // Aici puteți adăuga alte funcționalități pentru perifericele
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}