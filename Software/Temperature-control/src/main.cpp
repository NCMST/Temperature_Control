#include <Arduino.h>
#include <GyverPortal.h>
#include "screenMethods.h"
#include "webInterface.h"
#include "config.h"
#include "data.h" // Asigură-te că ai definit structura Data aici

// Declararea funcțiilor
void displayTask(void *pvParameters);
void displayGraph(void *pvParameters);
void PIDControl(void *pvParameters);

extern GyverPortal ui; // Declararea variabilei ui ca extern
extern GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> sens;

TaskHandle_t DisplayGraphTask, ControlTemperatureTask;

Data dataT;

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    xTaskCreate(displayTask, "Display Task", 10000, new Screen(), 1, NULL);
    xTaskCreatePinnedToCore(displayGraph, "Display Graph Task", 10000, NULL, 1, &DisplayGraphTask, 0);
    xTaskCreatePinnedToCore(PIDControl, "Implement PID algorithm", 10000, NULL, 1, &ControlTemperatureTask, 0);

    ui.attachBuild(build);
    ui.attach(action);
    ui.start();
}

void loop() {
    ui.tick();
}

// Definirea funcțiilor
void displayTask(void *pvParameters) {
    Screen *screen = (Screen *)pvParameters;
    screen->initialize(); // Inițializarea ecranului
    while (true) {
        screen->clearDisplay();
        screen->printText(0, 0, 1, "Temperatura");
        screen->printText(0, 20, 1, "O:" + String(dataT.readNTCTemp()));
        // Serial.println(dataT.readNTCTemp());
        screen->printText(60, 20, 1, "I:" + String(dataT.readKTemp()));
        Serial.println(dataT.readKTemp());
        screen->printText(40, 40, 1, "S:" + String(dataT.set_Temp));
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Afișează textul timp de 2 secunde
    }
}

void displayGraph(void *pvParameters) {
    for (;;) {
        if (dataT.flag[START_FLAG_INDEX]) {
            GPaddInt(dataT.K_Temp, dataT.temp[0], NUMBER_OF_TEMP_VALUE);
            GPaddInt(dataT.set_Temp, dataT.temp[1], NUMBER_OF_TEMP_VALUE);
            GPaddUnixS(5, dataT.dates, NUMBER_OF_TEMP_VALUE);
            dataT.list += "\n" + String(dataT.K_Temp) + ", " + String(dataT.set_Temp);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void PIDControl(void *pvParameters) {
    dataT.Kp = KP_VALUE;
    dataT.Ki = KI_VALUE;
    dataT.Kd = KD_VALUE;
    
    for (;;) {
        if (dataT.flag[START_FLAG_INDEX]) {
            dataT.input = dataT.readKTemp();
            digitalWrite(PWM_PIN, dataT.getResult());
        }
        vTaskDelay(PERIOD_VALUE / portTICK_PERIOD_MS);
    }
}