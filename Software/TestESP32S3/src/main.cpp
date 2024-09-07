#include "main.h"

WebServer server(80);

ESP32WiFiConfig wifiConfig(ap_ssid, ap_password, buttonPin, server);

/**
 * @brief InternetTask
 * 
 * @param pvParameters
 * @details Task to handle internet operations
 * @note The task is created on Core 0 
 */
void InternetTask(void *pvParameters) {
    ESP32WiFiConfig* wifiConfig = static_cast<ESP32WiFiConfig*>(pvParameters);
    wifiConfig->setup();  // Perform initial setup
    for (;;) {
        wifiConfig->loop();  // Handle server operations
        vTaskDelay(10 / portTICK_PERIOD_MS);  // Yield control
    }
}

/**
 * @brief PeripheralTask
 * 
 * @details Task to handle peripheral operations
 * @note The task is created on Core 1
 * @param pvParameters 
 */
void PeripheralTask(void *pvParameters) {
    for (;;) {
        // Code to handle peripherals
        vTaskDelay(100 / portTICK_PERIOD_MS);  // Adjust delay as needed
    }
}

/**
 * @brief setup
 * 
 * @return void
 * 
 * @details
 * 
 * 
 */
void setup() {
    // Create a task for internet operations on Core 0
    xTaskCreatePinnedToCore(
        InternetTask,     // Task function
        "InternetTask",   // Name of the task
        4096,             // Stack size
        &wifiConfig,      // Parameter to pass
        1,                // Priority
        NULL,             // Task handle
        0                 // Core 0
    );

    // Create a task for peripheral operations on Core 1
    xTaskCreatePinnedToCore(
        PeripheralTask,   // Task function
        "PeripheralTask", // Name of the task
        4096,             // Stack size
        NULL,             // Parameter to pass
        1,                // Priority
        NULL,             // Task handle
        1                 // Core 1
    );
}

/**
 * @brief loop
 * 
 * @return void
 * 
 * @details
 * 
 * 
 */
void loop() {
    
}