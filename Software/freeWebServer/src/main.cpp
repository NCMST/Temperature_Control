/**
 * @file main.cpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief Main file for the smart thermostat project.
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.hpp"

/**
 * @brief Task handles for web server, temperature reading, display, and PID control.
 * 
 */
TaskHandle_t WebServerTaskHandle, TemperatureTaskHandle, DisplayTaskHandle, PIDTaskHandle;

/**
 * @brief Current temperature data.
 * 
 */
TemperatureData currentTemperature;
/**
 * @brief Semaphore for temperature data access.
 * 
 */
SemaphoreHandle_t temperatureMutex;

/**
 * @brief Flag for zero crossing detection.
 * 
 */
volatile bool zeroCross = false;

/**
 * @brief Flag for turning off PID output.
 * 
 */
volatile bool turnOffRequest = false;

/**
 * @brief Global variable for the PID controller.
 * 
 * This variable is used to store the PID controller object.
 * and is accessed by the PID task.
 * also are modified by the web server task.
 */
float kp = 0.0, ki = 0.0, kd = 0.0;

/**
 * @brief ISR for detecting zero crossing.
 * 
 * This ISR is triggered at each zero crossing of the AC signal.
 * It sets the zeroCross flag to true, which is used by the PID task
 * to synchronize the triac control with the AC signal.
 */
void IRAM_ATTR zeroCrossISR()
{
    zeroCross = true; // Set the flag at each zero crossing
}

/**
 * @brief Setup function to initialize tasks and resources.
 * 
 * This function creates a mutex for temperature data access and initializes
 * tasks for web server, display, temperature reading, and PID control.
 */
void setup()
{
    // Create a mutex for temperature data access
    temperatureMutex = xSemaphoreCreateMutex();
    if (temperatureMutex == NULL)
    {
        Serial.println("Failed to create mutex");
        while (1)
            ;
    }

    // Create tasks and pin them to specific cores
    xTaskCreatePinnedToCore(webServerTask, "runWebServer", SERVER_TASK_STACK_SIZE, NULL, 1, &WebServerTaskHandle, 1);
    xTaskCreatePinnedToCore(displayTask, "runDisplay", DISPLAY_TASK_STACK_SIZE, new Screen(), 1, &DisplayTaskHandle, 0);
    xTaskCreatePinnedToCore(temperatureTask, "runTemperature", TEMPERATURE_TASK_STACK_SIZE, NULL, 1, &TemperatureTaskHandle, 0);
    xTaskCreatePinnedToCore(pidTaskHandle, "settemperature", PID_TASK_STACK_SIZE, NULL, 1, &PIDTaskHandle, 0);
}

/**
 * @brief Main loop function.
 * 
 * This function is intentionally left empty as tasks are handled by FreeRTOS.
 */
void loop() {}

// Core 1
/**
 * @brief Task for handling web server operations.
 * 
 * This task manages the web server, handles client requests, updates temperature data,
 * and logs information periodically.
 * 
 * @param pvParameters Parameters for the task (not used).
 */
void webServerTask(void *pvParameters)
{
    WebServerManager webServer(AP_SSID, AP_PASS, AP_SSID_WORK, AP_PASS_WORK);

    if (webServer.begin())
    {
        webServer.setupWiFIRouter(WF_SSID, WF_PASS);
    }

    TickType_t lastTime = xTaskGetTickCount();
    TickType_t lastTime_2 = xTaskGetTickCount();
    TickType_t lastTime_3 = xTaskGetTickCount();

    for (;;)
    {
        webServer.handleClient();

        // Sending the temperature data to the web server every second
        if (xTaskGetTickCount() - lastTime > SECOND)
        {
            // Reading the temperature data from the web server
            if (xSemaphoreTake(temperatureMutex, portMAX_DELAY))
            {
                currentTemperature.setpoint_temperature = webServer.getStetTemperature();
                currentTemperature.startFlag = webServer.getStartFlag();
                currentTemperature.setTime = webServer.getSetTime();

                kp, ki, kd = webServer.getPIDconstants();

                // Sending the temperature data to the web server
                webServer.setTemperatureData(currentTemperature);
                xSemaphoreGive(temperatureMutex);
            }

            lastTime = xTaskGetTickCount();
        }

        // Upload data in list.csv every minute
        if (xTaskGetTickCount() - lastTime_3 > MINUT)
        {
            if (currentTemperature.startFlag)
            {
                if (xSemaphoreTake(temperatureMutex, portMAX_DELAY))
                {
                    webServer.updateCSV(currentTemperature.inside_temperature, currentTemperature.setpoint_temperature, currentTemperature.setTime - currentTemperature.realTime);
                    xSemaphoreGive(temperatureMutex);
                }
            }

            lastTime_3 = xTaskGetTickCount();
        }

        if (xTaskGetTickCount() - lastTime_2 > LOGS_OFFSET)
        {
            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            if (LOG_MESSAGE)
            {
                Serial.println("Set time: " + String(webServer.getSetTime()));
                Serial.println("Set temperature: " + String(currentTemperature.setpoint_temperature));
                Serial.println("WebServer stack high water mark: " + String(uxHighWaterMark));
            }

            lastTime_2 = xTaskGetTickCount();
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

// core 0
/**
 * @brief Task for handling display operations.
 * 
 * This task manages the display, shows IP address, SSID, and temperature information,
 * and logs information periodically.
 * 
 * @param pvParameters Parameters for the task (Screen object).
 */
/**
 * @brief Task to display information on the screen.
 * 
 * This task initializes the screen and continuously updates it with the current
 * WiFi status and temperature information. It handles both WiFi station and 
 * access point modes, displaying the IP address, SSID, and temperature readings.
 * 
 * @param pvParameters Pointer to the Screen object to be used for display.
 * 
 * The task performs the following actions:
 * - Initializes the screen.
 * - Starts the serial communication.
 * - Checks the WiFi connection status and displays the IP address and SSID if connected.
 * - Displays temperature information including inside temperature, outside temperature,
 *   setpoint temperature, and the status of the start flag.
 * - Handles client connections in access point mode and displays a message when a client is connected.
 * - Logs the stack high water mark periodically for debugging purposes.
 * 
 * The task runs indefinitely with a delay of 0.5 seconds between iterations.
 */
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

            // Display temperature information
            screen->clearDisplay();
            if (xSemaphoreTake(temperatureMutex, portMAX_DELAY))
            {
                screen->printText(10, 0, 1, "Set time: " + String(currentTemperature.setTime - currentTemperature.realTime));
                screen->printText(0, 20, 1, "I_T: " + String(currentTemperature.inside_temperature, 2));
                screen->printText(SCREEN_WIDTH / 2, 20, 1, "O_T: " + String(currentTemperature.outside_temperature, 2));
                screen->printText(0, 40, 1, "S_T: " + String(currentTemperature.setpoint_temperature));
                currentTemperature.startFlag ? screen->printText(80, 40, 1, "ON") : screen->printText(80, 40, 1, "OFF");
                xSemaphoreGive(temperatureMutex);
            }
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
                if (xSemaphoreTake(temperatureMutex, portMAX_DELAY)) // Check if the mutex is available
                {
                    screen->printText(10, 0, 1, "Set time: " + String(currentTemperature.setTime - currentTemperature.realTime));
                    screen->printText(0, 20, 1, "I_T: " + String(currentTemperature.inside_temperature, 2));
                    screen->printText(SCREEN_WIDTH / 2, 20, 1, "O_T: " + String(currentTemperature.outside_temperature, 2));
                    screen->printText(0, 40, 1, "S_T: " + String(currentTemperature.setpoint_temperature));
                    currentTemperature.startFlag ? screen->printText(80, 40, 1, "ON") : screen->printText(80, 40, 1, "OFF");
                    xSemaphoreGive(temperatureMutex); // Release the mutex
                }
            }
            else
            {
                screen->printText(0, 60, 1, "Waiting for client...");
                printIP = false; // Reset printIP to allow re-display of IP on reconnection
            }
        }
        else
        {
            printIP = false; // Reset printIP to allow re-display of IP on reconnection
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

        vTaskDelay(pdMS_TO_TICKS(500)); // 0.5 second delay
    }
}

/**
 * @brief Task for reading temperature data.
 * 
 * This task reads inside and outside temperature data, updates the global temperature
 * structure, and logs information periodically.
 * 
 * @param pvParameters Parameters for the task (not used).
 */
void temperatureTask(void *pvParameters)
{
    Temperature temperature_data;

    // Initialize setpoint temperature to zero
    if (xSemaphoreTake(temperatureMutex, portMAX_DELAY))
    {
        currentTemperature.setpoint_temperature = 0;
        xSemaphoreGive(temperatureMutex);
    }

    Serial.begin(BAUD_RATE);

    TickType_t lastTime = xTaskGetTickCount();

    for (;;)
    {
        // Protect access to the global variable with a mutex
        if (xSemaphoreTake(temperatureMutex, portMAX_DELAY))
        {
            currentTemperature.inside_temperature = temperature_data.readKTemp();
            currentTemperature.outside_temperature = temperature_data.readNTCTemp();
            currentTemperature.timestamp = String(millis());

            xSemaphoreGive(temperatureMutex);
        }

        if (xTaskGetTickCount() - lastTime > LOGS_OFFSET)
        {
            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            if (LOG_MESSAGE)
                Serial.println("Temperature stack high water mark: " + String(uxHighWaterMark));

            if (LOG_MESSAGE)
            {
                float kTemp = temperature_data.readKTemp();
                float ntcTemp = temperature_data.readNTCTemp();

                Serial.println("K_Temperature, NTC_Temperature: " + String(kTemp) + " " + String(ntcTemp));
            }

            lastTime = xTaskGetTickCount();
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // 0.2 second delay between readings
    }
}

/**
 * @brief Task for PID control and triac management.
 * 
 * This task handles the PID control for temperature regulation, manages the triac
 * state based on zero crossing detection, and logs information periodically.
 * 
 * @param pvParameters Parameters for the task (not used).
 */
void pidTaskHandle(void *pvParameters)
{
    pinMode(MOC_PIN, OUTPUT);
    pinMode(ZCD_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ZCD_PIN), zeroCrossISR, FALLING); // Detect zero crossing

    PID pid(kp, ki, kd);
    pid.setLimits(0, 1);

    TickType_t lastTime = xTaskGetTickCount();
    TickType_t lastTime_work = xTaskGetTickCount();

    currentTemperature.realTime = 0;
    uint8_t output = 0;
    bool triacState = false; // Current state of the triac

    for (;;)
    {
        if (currentTemperature.startFlag)
        {
            pid.setKp(kp);
            pid.setKi(ki);
            pid.setKd(kd);
            
            output = pid.compute(currentTemperature.setpoint_temperature, currentTemperature.inside_temperature);

            if (output == 1)
            {
                // PID wants the heating on, the triac remains active
                digitalWrite(MOC_PIN, HIGH);
                triacState = true;
            }
            else if (output == 0)
            {
                // PID requests shutdown, but we wait for zero crossing
                turnOffRequest = true;
            }

            if (turnOffRequest && zeroCross)
            {
                // At the first zero crossing after shutdown is requested
                digitalWrite(MOC_PIN, LOW);
                triacState = false;
                turnOffRequest = false;
                zeroCross = false; // Reset zero crossing flag
            }

            // Update remaining time
            if (xTaskGetTickCount() - lastTime_work > MINUT)
            {
                if (abs(currentTemperature.setpoint_temperature - currentTemperature.inside_temperature) <= 3 ||
                    currentTemperature.setpoint_temperature > currentTemperature.inside_temperature)
                {

                    if (currentTemperature.setTime - currentTemperature.realTime > 0)
                    {
                        currentTemperature.realTime++;
                    }
                    else if (currentTemperature.setTime - currentTemperature.realTime == 0)
                    {
                        if (xSemaphoreTake(temperatureMutex, portMAX_DELAY))
                        {
                            currentTemperature.startFlag = false;
                            xSemaphoreGive(temperatureMutex);
                        }
                        digitalWrite(MOC_PIN, 0);
                    }
                }
                lastTime_work = xTaskGetTickCount();
            }
        }
        else
        {
            digitalWrite(MOC_PIN, 0);
            triacState = false;
        }

        // Logs every 10 seconds
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (LOG_MESSAGE)
            if (xTaskGetTickCount() - lastTime > LOGS_OFFSET)
            {
                Serial.print("PID output: ");
                Serial.println(output);
                Serial.print("Triac State: ");
                Serial.println(triacState);
                Serial.print("Remained time: ");

                Serial.println(currentTemperature.setTime - currentTemperature.realTime);
                Serial.println("PID stack high water mark: " + String(uxHighWaterMark));
            }
        lastTime = xTaskGetTickCount();

        vTaskDelay(pdMS_TO_TICKS(20)); // Short delay to avoid blocking the task
    }
}
