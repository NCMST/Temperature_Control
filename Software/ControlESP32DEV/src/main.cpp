#include "main.h"

TaskHandle_t DisplayTask, DisplayGraphTask, PID_ControlTask;

void setup()
{
    Serial.begin(115200);

    initWifi();

    xTaskCreatePinnedToCore(displayTask, "displayTask", 10000, new Screen(), 1, &DisplayTask, 1);
    xTaskCreatePinnedToCore(displayGraphTask, "Display Graph Task", 10000, NULL, 1, &DisplayGraphTask, 0);
    xTaskCreatePinnedToCore(PIDControlTask, "Implement PID algorithm", 10000, NULL, 1, &PID_ControlTask, 0);

    ui.attachBuild(build);
    ui.attach(action);
    ui.start();
}

void loop()
{
    ui.tick();
}

/**
 * @brief Task for ssd1306 display
 * @date 05/11/2024
 * @author Catalin
 * @param pvParameters
 */
void displayTask(void *pvParameters)
{
    Screen *screen = (Screen *)pvParameters;
    screen->init(); // Inițializarea ecranului
    while (true)
    {
        screen->clearDisplay();

        // Obține timpul formatat
        String timeText = "t:" + formatTime(startTime);
        int timeTextWidth = strlen(timeText.c_str()) * 6;                      // Aproximarea lățimii (6 pixeli per caracter)
        screen->printText((SCREEN_WIDTH - timeTextWidth) / 2, 0, 1, timeText); // Centrarea textului

        // Afișează celelalte informații
        String kTempText = "K:" + String(dataT.readKTemp());
        screen->printText(0, 20, 1, kTempText); // Centrarea textului

        String setTempText = "S:" + String(set_Temp);
        screen->printText(70, 20, 1, setTempText); // Centrarea textului

        if (flag[START_FLAG_INDEX])
        {
            String onOffText = "ON";
            screen->printText((SCREEN_WIDTH - strlen(onOffText.c_str()) * 6) / 2, 40, 1, onOffText); // Centrarea textului
        }
        else
        {
            String onOffText = "OFF";
            screen->printText((SCREEN_WIDTH - strlen(onOffText.c_str()) * 6) / 2, 40, 1, onOffText); // Centrarea textului
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS); // Afișează textul timp de 2 secunde
    }
}

/**
 * @brief Colect data for graph
 * @date 05/11/2024
 * @author Catalin
 * @param pvParameters
 */
void displayGraphTask(void *pvParameters)
{
    for (;;)
    {
        if (flag[START_FLAG_INDEX])
        {
            GPaddInt(dataT.readKTemp(), temp[0], NUMBER_OF_TEMP_VALUE);
            GPaddInt(set_Temp, temp[1], NUMBER_OF_TEMP_VALUE);
            GPaddUnixS(5, dates, NUMBER_OF_TEMP_VALUE);
            list += "\n" + String(dataT.readKTemp()) + ", " + String(set_Temp) + ", " + formatTime(startTime);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Task for implementing PID algorithm
 * @date 07/11/2024
 * @author Catalin  
 * @param pvParameters
 */
void PIDControlTask(void *pvParameters)
{
    pinMode(ZD_PIN, INPUT);

    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);

    dataT.setDirection(NORMAL);
    dataT.setLimit(0, 255); // 0 - 255 8 biți

    dataT.setKp(KP_VALUE);
    dataT.setKi(KI_VALUE);
    dataT.setKd(KD_VALUE);

    dataT.setLimit(0, 255);

    unsigned long startTime = 0;
    bool timerStarted = false;

    bool sensorValue, lastSensorValue;

    for (;;)
    {
        lastSensorValue = sensorValue;
        sensorValue = digitalRead(ZD_PIN);

        if (flag[START_FLAG_INDEX])
        {
            dataT.setInput(dataT.readKTemp());
            int pwmValue = dataT.getResult();

            if (pwmValue == 0)
            {
                if (sensorValue != lastSensorValue)
                {
                    digitalWrite(PWM_PIN, LOW);
                }
            }
            else
                ledcWrite(PWM_CHANNEL, pwmValue);

            if (!timerStarted && (dataT.getInput() >= (set_Temp - 5)) && (dataT.getInput() <= (set_Temp + 5)))
            {
                startTime = millis();
                timerStarted = true;
            }

            if (timerStarted)
            {
                startTime = millis() - startTime;

                if (startTime >= setTime * 60000)
                {
                    flag[START_FLAG_INDEX] = false;
                    Serial.println("Timpul setat a fost atins!");
                }
            }
        }
        else
        {
            digitalWrite(PWM_PIN, LOW);
            ledcWrite(PWM_CHANNEL, 0);
        }

        vTaskDelay(PERIOD_VALUE / portTICK_PERIOD_MS);
    }
}