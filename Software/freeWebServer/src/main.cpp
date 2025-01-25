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

    TickType_t lastTime = xTaskGetTickCount();
    TickType_t lastTime_2 = xTaskGetTickCount();

    for (;;)
    {
        webServer.handleClient();

        if (xTaskGetTickCount() - lastTime > SECOND)
        {
            currentTemperature.setpoint_temperature = webServer.getStetTemperature();

            currentTemperature.startFlag = webServer.getStartFlag();

            currentTemperature.setTime = webServer.getSetTime();

            webServer.setTemperatureData(currentTemperature);

            lastTime = xTaskGetTickCount();
        }

        if (xTaskGetTickCount() - lastTime_2 > LOGS_OFFSET)
        {
            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            if (LOG_MESSAGE)
            {
                Serial.println("Seted time: " + String(webServer.getSetTime()));
                Serial.println("Seted temperatureL: " + String(currentTemperature.setpoint_temperature));
                Serial.println("WebServer stack high water mark: " + String(uxHighWaterMark));
            }

            lastTime_2 = xTaskGetTickCount();
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
    bool clientConnected = false;

    TickType_t lastTime = xTaskGetTickCount();

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
            screen->printText(10, 0, 1, "Seted time: " + String(currentTemperature.setTime - currentTemperature.realTime));
            screen->printText(0, 20, 1, "I_T: " + String(currentTemperature.inside_temperature, 2));
            screen->printText(SCREEN_WIDTH / 2, 20, 1, "O_T: " + String(currentTemperature.outside_temperature, 2));
            screen->printText(0, 40, 1, "S_T: " + String(currentTemperature.setpoint_temperature));
            currentTemperature.startFlag ? screen->printText(80, 40, 1, "ON") : screen->printText(80, 40, 1, "OFF");
        }
        else if (WiFi.softAPgetStationNum() > 0)
        {
            if (!clientConnected)
            {
                if (!printIP)
                {
                    screen->printText(0, 60, 1, "Client connected");
                    clientConnected = true;
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    printIP = true;
                }
                screen->clearDisplay();
                screen->printText(10, 0, 1, "Seted time: " + String(currentTemperature.setTime - currentTemperature.realTime));
                screen->printText(0, 20, 1, "I_T: " + String(currentTemperature.inside_temperature, 2));
                screen->printText(SCREEN_WIDTH / 2, 20, 1, "O_T: " + String(currentTemperature.outside_temperature, 2));
                screen->printText(0, 40, 1, "S_T: " + String(currentTemperature.setpoint_temperature));
                currentTemperature.startFlag ? screen->printText(80, 40, 1, "ON") : screen->printText(80, 40, 1, "OFF");
            }
            else
            {
                screen->printText(0, 60, 1, "Waiting for client...");
                printIP = false; // Resetăm printIP pentru a permite reafișarea IP-ului la reconectare
            }
        }
        else
        {
            printIP = false; // Resetăm printIP pentru a permite reafișarea IP-ului la reconectare
            screen->clearDisplay();
            screen->printText(0, 0, 1, "Connecting to WiFi...");
        }

        if (xTaskGetTickCount() - lastTime > LOGS_OFFSET)
        {
            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            if (LOG_MESSAGE)
                Serial.println("Screen stack high water mark: " + String(uxHighWaterMark));
            lastTime = xTaskGetTickCount();
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 0.5 secundă
    }
}

void temperatureTask(void *pvParameters)
{
    Temperature temperature_data;

    currentTemperature.setpoint_temperature = 0;

    Serial.begin(BAUD_RATE);

    TickType_t lastTime = xTaskGetTickCount();

    for (;;)
    {
        currentTemperature.inside_temperature = temperature_data.readKTemp();
        currentTemperature.outside_temperature = temperature_data.readNTCTemp();
        currentTemperature.timestamp = String(millis());

        if (xTaskGetTickCount() - lastTime > LOGS_OFFSET)
        {
            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            if (LOG_MESSAGE)
                Serial.println("Temperature stack high water mark: " + String(uxHighWaterMark));

            if (LOG_MESSAGE)
            {
                Serial.println("K_Temperature, NTC_Temperature: " + String(temperature_data.readKTemp()) + " " + String(temperature_data.readNTCTemp()));
            }
            lastTime = xTaskGetTickCount();
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // Delay de 0.1 secundă între citiri
    }
}

void pidTaskHandle(void *pvParameters)
{
    pinMode(MOC_PIN, OUTPUT);
    pinMode(ZCD_PIN, INPUT);

    PID pid(1, 0.1, 0.1);
    pid.setLimits(0, 1);

    TickType_t lastTime = xTaskGetTickCount();
    TickType_t lastTime_work = xTaskGetTickCount();

    currentTemperature.realTime = 0;
    uint8_t output = 0;

    for (;;)
    {
        // evry 0.2 sec
        if (currentTemperature.startFlag)
        {
            output = pid.compute(currentTemperature.setpoint_temperature, currentTemperature.inside_temperature);
            digitalWrite(MOC_PIN, output);

            // evry 1 min
            if (xTaskGetTickCount() - lastTime_work > MINUT)
            {
                // Check if the temperature is within the error range
                if (abs(currentTemperature.setpoint_temperature - currentTemperature.inside_temperature) <= 3 || currentTemperature.setpoint_temperature > currentTemperature.inside_temperature)
                {
                    // Decrease the set time
                    if (currentTemperature.setTime - currentTemperature.realTime > 0)
                    {
                        currentTemperature.realTime++;
                    }
                    else
                    {
                        // Stop heating when time reaches 0
                        currentTemperature.startFlag = false;
                        digitalWrite(MOC_PIN, 0);
                    }
                }
                lastTime_work = xTaskGetTickCount();
            }
        }
        else
        {
            digitalWrite(MOC_PIN, 0);
        }

        // evry 10 sec
        if (xTaskGetTickCount() - lastTime > LOGS_OFFSET)
        {
            Serial.println("PID output: " + String(output));
            Serial.println("Remained time: " + String(currentTemperature.setTime - currentTemperature.realTime));

            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            if (LOG_MESSAGE)
                Serial.println("PID stack high water mark: " + String(uxHighWaterMark));
            lastTime = xTaskGetTickCount();
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // Delay de 0.2 secund între citiri
    }
}