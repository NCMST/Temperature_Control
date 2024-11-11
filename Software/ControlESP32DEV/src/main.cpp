#include "main.h"

TaskHandle_t DisplayTask, DisplayGraphTask, PID_ControlTask;

WebInterface wf_ui;

Temperature dataT;

void setup()
{
    Serial.begin(115200);

    wf_ui.initWifi();

    xTaskCreatePinnedToCore(displayTask, "displayTask", 10000, new Screen(), 1, &DisplayTask, 1);
    xTaskCreatePinnedToCore(displayGraphTask, "Display Graph Task", 10000, NULL, 1, &DisplayGraphTask, 0);
    xTaskCreatePinnedToCore(PIDControlTask, "Implement PID algorithm", 10000, NULL, 1, &PID_ControlTask, 0);


    wf_ui.ui.start();

}

void loop()
{
    wf_ui.tick();
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
        String timeText = "t:" + wf_ui.formatTime(wf_ui.startTime);
        int timeTextWidth = strlen(timeText.c_str()) * 6;                      // Aproximarea lățimii (6 pixeli per caracter)
        screen->printText((SCREEN_WIDTH - timeTextWidth) / 2, 0, 1, timeText); // Centrarea textului

        // Afișează celelalte informații
        String kTempText = "K:" + String(dataT.readKTemp());
        screen->printText(0, 20, 1, kTempText); // Centrarea textului

        String setTempText = "S:" + String(wf_ui.set_Temp);
        screen->printText(70, 20, 1, setTempText); // Centrarea textului

        if (wf_ui.flag[START_FLAG_INDEX])
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
        if (wf_ui.flag[START_FLAG_INDEX])
        {
            GPaddInt(dataT.readKTemp(), wf_ui.temp[0], NUMBER_OF_TEMP_VALUE);
            GPaddInt(wf_ui.set_Temp, wf_ui.temp[1], NUMBER_OF_TEMP_VALUE);
            GPaddUnixS(5, wf_ui.dates, NUMBER_OF_TEMP_VALUE);
            wf_ui.list += "\n" + String(dataT.readKTemp()) + ", " + String(wf_ui.set_Temp) + ", " + wf_ui.formatTime(wf_ui.startTime);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief
 *
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

        if (wf_ui.flag[START_FLAG_INDEX])
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

            if (!timerStarted && (dataT.getInput() >= (wf_ui.set_Temp - 5)) && (dataT.getInput() <= (wf_ui.set_Temp + 5)))
            {
                startTime = millis();
                timerStarted = true;
            }

            if (timerStarted)
            {
                startTime = millis() - startTime;

                if (startTime >= wf_ui.setTime * 60000)
                {
                    wf_ui.flag[START_FLAG_INDEX] = false;
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