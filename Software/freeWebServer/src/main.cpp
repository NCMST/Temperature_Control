#include "main.hpp"

TaskHandle_t WebServerTaskHandle, TemperatureTaskHandle, DisplayTaskHandle;

void setup()
{
    temperatureQueue = xQueueCreate(10, sizeof(TemperatureData));

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
            TemperatureData tempData;
            if (xQueueReceive(temperatureQueue, &tempData, 0) == pdTRUE)
            {
                webServer.setTemperatureData(tempData);
            }
            else
            {
                Serial.println("No temperature data received.");
            }

            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
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

            TemperatureData tempData;

            if (xQueueReceive(temperatureQueue, &tempData, 0) == pdTRUE)
            {
                screen->clearDisplay();
                screen->printText(0, 0, 1, "Seted time:");

                screen->printText(0, 20, 1, "I_T:" + String(tempData.inside_temperature, 2));
                screen->printText(SCREEN_WIDTH / 2, 20, 1, "O_T:" + String(tempData.outside_temperature, 2));
            }
            else
            {
                Serial.println("No temperature data received.");
            }

            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            Serial.println("Screen stack high water mark: " + String(uxHighWaterMark));
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay de 1 secundă
    }
}

void temperatureTask(void *pvParameters)
{
    Temperature temperature_data;
    Serial.begin(BAUD_RATE);

    for (;;)
    {
        TemperatureData tempData;
        tempData.inside_temperature = temperature_data.readKTemp();
        tempData.outside_temperature = temperature_data.readNTCTemp();
        tempData.timestamp = String(millis());

        if (xQueueSend(temperatureQueue, &tempData, portMAX_DELAY) != pdPASS)
        {
            Serial.println("Failed to send temperature data to queue");
        }

        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        Serial.println("Temperature stack high water mark: " + String(uxHighWaterMark));

        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 1 secundă între citiri
    }
}