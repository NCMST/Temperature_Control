#include "main.hpp"

TaskHandle_t WebServerTaskHandle, TemperatureTaskHandle, DisplayTaskHandle, PIDTaskHandle;

TemperatureData currentTemperature;

void setup()
{
    xTaskCreatePinnedToCore(webServerTask, "runWebServer", SERVER_TASK_STACK_SIZE, NULL, 1, &WebServerTaskHandle, 1);
    xTaskCreatePinnedToCore(displayTask, "runDisplay", DISPLAY_TASK_STACK_SIZE, new Screen(), 1, &DisplayTaskHandle, 0);
    xTaskCreatePinnedToCore(temperatureTask, "runTemperature", TEMPERATURE_TASK_STACK_SIZE, NULL, 1, &TemperatureTaskHandle, 0);
    // PID
    xTaskCreatePinnedToCore(pidTaskHandle, "settemperature", PID_TASK_STACK_SIZE, NULL, 1, &PIDTaskHandle, 0);
}

void loop() {}

void webServerTask(void *pvParameters)
{
    WebServerManager webServer(AP_SSID, AP_PASS, AP_SSID_WORK, AP_PASS_WORK);

    if (webServer.begin())
    {
        webServer.setupWiFIRouter(WF_SSID, WF_PASS);
    }

    const TickType_t Ofset = pdMS_TO_TICKS(1000);
    TickType_t lastTime = xTaskGetTickCount();

    for (;;)
    {
        webServer.handleClient();

        if (xTaskGetTickCount() - lastTime > Ofset)
        {
            webServer.setTemperatureData(currentTemperature);

            currentTemperature.setpoint_temperature = webServer.getStetTemperature();

            currentTemperature.startFlag = webServer.getStartFlag();

            currentTemperature.setTime = webServer.getSetTime();

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
        if (WiFi.status() == WL_CONNECTED)
        {
            if (!printIP)
            {
                screen->clearDisplay();
                screen->printText(0, 0, 1, "IP: " + WiFi.localIP().toString());
                screen->printText(0, 30, 1, "SSID: " + WiFi.SSID());
                vTaskDelay(pdMS_TO_TICKS(5000));
                printIP = true;
            }

            // Afișează informațiile despre temperaturi
            screen->clearDisplay();
            screen->printText(0, 0, 1, "Seted time: " + String(currentTemperature.setTime));
            screen->printText(0, 20, 1, "I_T: " + String(currentTemperature.inside_temperature, 2));
            screen->printText(SCREEN_WIDTH / 2, 20, 1, "O_T: " + String(currentTemperature.outside_temperature, 2));
            screen->printText(0, 40, 1, "S_T: " + String(currentTemperature.setpoint_temperature));
            currentTemperature.startFlag ? screen->printText(80, 40, 1, "ON") : screen->printText(80, 40, 1, "OFF");
        }
        else
        {
            printIP = false; // Resetăm printIP pentru a permite reafișarea IP-ului la reconectare
            screen->clearDisplay();
            screen->printText(0, 0, 1, "Connecting to WiFi...");
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

    currentTemperature.setpoint_temperature = 0;

    Serial.begin(BAUD_RATE);

    for (;;)
    {
        currentTemperature.inside_temperature = temperature_data.readKTemp();
        currentTemperature.outside_temperature = temperature_data.readNTCTemp();
        currentTemperature.timestamp = String(millis());

        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (LOG_MESSAGE)
            Serial.println("Temperature stack high water mark: " + String(uxHighWaterMark));

        if (LOG_MESSAGE)
        {
            Serial.println(String(temperature_data.readKTemp()) + " " + String(temperature_data.readNTCTemp()));
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay de 1 secundă între citiri
    }
}

void pidTaskHandle(void *pvParameters)
{
    pinMode(MOC_PIN, OUTPUT);
    pinMode(ZCD_PIN, INPUT);

    PID pid(1, 0.1, 0.1);
    pid.setLimits(0, 1);

    for (;;)
    {
        if (currentTemperature.startFlag)
        {
            uint8_t output = pid.compute(currentTemperature.setpoint_temperature, currentTemperature.inside_temperature);
            digitalWrite(MOC_PIN, output);
        }
        else
        {
            analogWrite(MOC_PIN, 0);
        }

        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (LOG_MESSAGE)
            Serial.println("PID stack high water mark: " + String(uxHighWaterMark));

        vTaskDelay(pdMS_TO_TICKS(200)); // Delay de 0.2 secund între citiri
    }
}