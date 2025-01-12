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
void initWifi(void);
void connectToWiFi(Screen& screen, const char* ssid, const char* password, unsigned long timeout);

extern GyverPortal ui; // Declararea variabilei ui ca extern
extern GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> sens;

TaskHandle_t DisplayGraphTask, ControlTemperatureTask;

Data dataT;

void setup()
{
    Serial.begin(115200);

    initWifi();

    xTaskCreate(displayTask, "Display Task", 10000, new Screen(), 1, NULL);
    xTaskCreatePinnedToCore(displayGraph, "Display Graph Task", 10000, NULL, 1, &DisplayGraphTask, 0);
    xTaskCreatePinnedToCore(PIDControl, "Implement PID algorithm", 10000, NULL, 1, &ControlTemperatureTask, 0);

    ui.attachBuild(build);
    ui.attach(action);
    ui.start();
}

void loop()
{
    ui.tick();
}

// Definirea funcțiilor
void displayTask(void *pvParameters)
{
    Screen *screen = (Screen *)pvParameters;
    screen->initialize(); // Inițializarea ecranului
    while (true)
    {
        screen->clearDisplay();

        // Obține timpul formatat
        String timeText = "t:" + formatTime(dataT.time);
        int timeTextWidth = strlen(timeText.c_str()) * 6;                      // Aproximarea lățimii (6 pixeli per caracter)
        screen->printText((SCREEN_WIDTH - timeTextWidth) / 2, 0, 1, timeText); // Centrarea textului

        // Afișează celelalte informații
        String kTempText = "K:" + String(dataT.readKTemp());
        screen->printText(0, 20, 1, kTempText); // Centrarea textului

        String setTempText = "S:" + String(dataT.set_Temp);
        screen->printText(70, 20, 1, setTempText); // Centrarea textului

        if (dataT.flag[START_FLAG_INDEX])
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

void displayGraph(void *pvParameters)
{
    for (;;)
    {
        if (dataT.flag[START_FLAG_INDEX])
        {
            GPaddInt(dataT.K_Temp, dataT.temp[0], NUMBER_OF_TEMP_VALUE);
            GPaddInt(dataT.set_Temp, dataT.temp[1], NUMBER_OF_TEMP_VALUE);
            GPaddUnixS(5, dataT.dates, NUMBER_OF_TEMP_VALUE);
            dataT.list += "\n" + String(dataT.K_Temp) + ", " + String(dataT.set_Temp) + ", " + formatTime(dataT.time);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void PIDControl(void *pvParameters) {
    pinMode(ZD_PIN, INPUT); // Setezi pinul ca intrare

    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL); // Asociază pinul cu canalul PWM


    dataT.setDirection(NORMAL);
    dataT.setLimits(0, 255); // 0 - 255 8 biți

    dataT.Kp = KP_VALUE;
    dataT.Ki = KI_VALUE;
    dataT.Kd = KD_VALUE;

    unsigned long startTime = 0; // Variabilă pentru a salva timpul de început
    bool timerStarted = false; // Flag pentru a verifica dacă timerul a fost început

    for (;;) {
        int sensorValue = digitalRead(ZD_PIN);

        if (dataT.flag[START_FLAG_INDEX]) {
            dataT.input = dataT.readKTemp();
            int pwmValue = constrain(dataT.getResult(), 0, 255); // Limitează între 0 și 255

            if (pwmValue == 0) { // Daca valoarea PWM este 0
                if (sensorValue == LOW) { // Daca valoarea senzorului este LOW
                    digitalWrite(PWM_PIN, LOW);
                }
            }

            // Controlul tiristorului
            ledcWrite(PWM_CHANNEL, pwmValue);

            // Verifică dacă temperatura este în intervalul dorit
            if (!timerStarted && (dataT.input >= (dataT.set_Temp - 5)) && (dataT.input <= (dataT.set_Temp + 5))) {
                startTime = millis();          // Salvează timpul de început
                timerStarted = true;           // Marchează că timerul a fost început
            }

            // Dacă timerul a fost început, salvează timpul în dataT.time
            if (timerStarted) {
                dataT.time = millis() - startTime; // Timpul scurs de la început

                // Verifică dacă timpul scurs depășește timpul setat
                if (dataT.time >= dataT.setTime * 60000) { // Convertim minute în milisecunde
                    dataT.flag[START_FLAG_INDEX] = false; // Oprește PID-ul
                    Serial.println("Timpul setat a fost atins!");
                }
            }
        } else {
            digitalWrite(PWM_PIN, LOW);
            ledcWrite(PWM_CHANNEL, 0); // Oprește PWM-ul
        }

        vTaskDelay(PERIOD_VALUE / portTICK_PERIOD_MS);
    }
}

void initWifi()
{
    Screen screen; // Instanță locală
    screen.initialize();

    WiFi.mode(WIFI_STA);

    // Încercare de conectare la AP_SSID_WORK
    connectToWiFi(screen, AP_SSID_WORK, AP_PASS_WORK, 10000); // 10 secunde timeout

    // Dacă nu s-a conectat, încearcă AP_SSID
    if (WiFi.status() != WL_CONNECTED)
    {
        connectToWiFi(screen, AP_SSID, AP_PASS, 10000); // 10 secunde timeout
    }

    // Dacă încă nu s-a conectat, intră în modul de router
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ROUTER_SSID, ROUTER_PASS); // Poți schimba numele și parola router-ului
        String ipAddress = WiFi.softAPIP().toString();
        
        screen.clearDisplay();
        screen.printText(0, 0, 1, "No Connection!");
        screen.printText(0, 20, 1, "Router IP: " + ipAddress);
        screen.display();
    }
    else
    {
        // Afișarea IP-ului pe ecran timp de 2 secunde
        String ipAddress = WiFi.localIP().toString();
        screen.clearDisplay();
        screen.printText(0, 0, 1, "Connected!");
        screen.printText(0, 20, 1, "IP: " + ipAddress);
        screen.display();

        delay(2000); // Afișează IP-ul timp de 2 secunde
    }
}

void connectToWiFi(Screen& screen, const char* ssid, const char* password, unsigned long timeout)
{
    WiFi.begin(ssid, password);
    
    screen.clearDisplay();
    screen.printText(0, 0, 1, "Connecting to " + String(ssid) + "...");
    screen.display();

    unsigned long startTime = millis();
    
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - startTime > timeout)
        {
            break; // Ieși din buclă dacă timpul a expirat
        }
        delay(500);
    }
}

