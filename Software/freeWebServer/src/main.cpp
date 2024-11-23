#include "main.hpp"

TaskHandle_t WebServerTaskHandle, TemperatureTaskHandle, DisplayTaskHandle;

TemperatureData currentTemperature;

void setup()
{
    xTaskCreatePinnedToCore(webServerTask, "runWebServer", SERVER_TASK_STACK_SIZE, NULL, 1, &WebServerTaskHandle, 1);
    xTaskCreatePinnedToCore(displayTask, "runDisplay", DISPLAY_TASK_STACK_SIZE, new Screen(), 1, &DisplayTaskHandle, 0);
    xTaskCreatePinnedToCore(temperatureTask, "runTemperature", TEMPERATURE_TASK_STACK_SIZE, NULL, 1, &TemperatureTaskHandle, 0);
}

void loop() {}

void webServerTask(void *pvParameters)
{
    WebServerManager webServer(AP_SSID, AP_PASS);
    webServer.begin();

    const TickType_t Ofset = pdMS_TO_TICKS(1000);
    TickType_t lastTime = xTaskGetTickCount();

    for (;;)
    {
        webServer.handleClient();

        if (xTaskGetTickCount() - lastTime > Ofset)
        {
            webServer.setTemperatureData(currentTemperature);

            currentTemperature.setpoint_temperature = webServer.getStetTemperature();
            
            if (LOG_MESSAGE)
                Serial.println(currentTemperature.setpoint_temperature);

            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            if (LOG_MESSAGE)
                Serial.println("WebServer stack high water mark: " + String(uxHighWaterMark));

            lastTime = xTaskGetTickCount();
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void displayTask(void *pvParameters)
{
    Screen *screen = (Screen *)pvParameters;
    screen->init();

    Serial.begin(BAUD_RATE);

    bool printIP = false;

    for (;;)
    {
        if (!printIP && WiFi.status() == WL_CONNECTED)
        {
            screen->clearDisplay();
            screen->printText(10, 0, 1, "IP: " + WiFi.localIP().toString());

            vTaskDelay(pdMS_TO_TICKS(5000));

            printIP = true;
        }
        else
        {
            screen->clearDisplay();
            screen->printText(0, 0, 1, "Seted time:");

            screen->printText(0, 20, 1, "I_T:" + String(currentTemperature.inside_temperature, 2));
            screen->printText(SCREEN_WIDTH / 2, 20, 1, "O_T:" + String(currentTemperature.outside_temperature, 2));

            screen->printText(0, 40, 1, "S_T:" + String(currentTemperature.setpoint_temperature));
        }

        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (LOG_MESSAGE)
            Serial.println("Screen stack high water mark: " + String(uxHighWaterMark));

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay de 1 secundă
    }
}

void temperatureTask(void *pvParameters)
{
    Temperature temperature_data;

    Serial.begin(BAUD_RATE);

    for (;;)
    {
        if (LOG_MESSAGE)
        {
            Serial.println(String(temperature_data.readKTemp()) + " " + String(temperature_data.readNTCTemp()));
        }

        currentTemperature.inside_temperature = temperature_data.readKTemp();
        currentTemperature.outside_temperature = temperature_data.readNTCTemp();
        currentTemperature.timestamp = String(millis());

        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (LOG_MESSAGE)
            Serial.println("Temperature stack high water mark: " + String(uxHighWaterMark));

        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 1 secundă între citiri
    }
}