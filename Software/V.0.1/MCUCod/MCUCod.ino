#include "screenMethods.h"

TaskHandle_t DisplayGraphTask, ControlTemperatureTask;

extern GyverPortal ui;

Data dataT;

void displayTask(void *pvParameters)
{
  Screen *screen = (Screen *)pvParameters;
  screen->initialize(); // Inițializarea ecranului
  Serial.println(WiFi.localIP());

  while (1)
  {
    // Afișați textul dorit pe ecran
    screen->clearDisplay();
    screen->printText(30, 0, 1, "Temperatura");
    // Serial.println(dataT.readNTCTemp());
    screen->printText(0, 20, 1, "O:" + String(dataT.readNTCTemp()));

    screen->printText(60, 20, 1, "I:" + String(dataT.readKTemp()));
    screen->printText(40, 40, 1, "S:" + String(dataT.set_Temp));

    // screen->printText(0, 40, 1, String(WiFi.localIP()));
    // Adăugați aici orice alte acțiuni pentru task-ul de afișare
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Afișați textul timp de 2 secunde
  }
}

void displayGraph(void *pvParameters)
{

  // ============LOOP===========
  for (;;)
  {
    if (dataT.flag[START_FLAG_INDEX])
    {
      GPaddInt(dataT.K_Temp, dataT.temp[0], NUMBER_OF_TEMP_VALUE);
      GPaddInt(dataT.set_Temp, dataT.temp[1], NUMBER_OF_TEMP_VALUE);
      GPaddUnixS(5, dataT.dates, NUMBER_OF_TEMP_VALUE);

      dataT.list += "\n" + String(dataT.K_Temp) + ", " + String(dataT.set_Temp);
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void PIDControl(void *pvParameters)
{
  dataT.Kp = KP_VALUE;
  dataT.Ki = KI_VALUE;
  dataT.Kd = KD_VALUE;
  dataT.setMode(ON_ERROR);
  dataT.setDirection(NORMAL);
  dataT.setLimits(MIN_PID_VALUE, MAX_PID_VALUE);
  dataT.setDt(PERIOD_VALUE);

  for (;;)
  {
    if (dataT.flag[START_FLAG_INDEX])
    {
      dataT.input = dataT.readKTemp();
      digitalWrite(PWM_PIN, dataT.getResult());
    }
    vTaskDelay(PERIOD_VALUE / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    // Serial.print(".");
  }

  xTaskCreate(displayTask, "Display Task", 10000, new Screen(), 0, NULL);
  xTaskCreatePinnedToCore(displayGraph, "Display Graph Task", 10000, NULL, 1, &DisplayGraphTask, 0);
  // // Funcția pentru task-ul de afișare // Numele task-ului// Dimensiunea stivei (modificați la nevoie) // Parametrii pentru task (obiectul Screen)// Prioritate (cu o valoare mai mare, task-ul rulează mai repede)
  xTaskCreatePinnedToCore(PIDControl, "Implement PID algorithm", 10000, NULL, 1, &ControlTemperatureTask, 0);

  // conetct and start site info
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void loop()
{
  ui.tick();
  vTaskDelay(1 / portTICK_PERIOD_MS);
}