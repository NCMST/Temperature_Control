#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <Update.h>

#include "personalserver.h"
#include "screen.h"
#include "temperature.h"
#include "control.h"

TaskHandle_t DisplayTextOnScreenTasck, DisplayGraphUITask, ControlTemperatureTask;

PersonalServer ps(120); // Set Config Portal Timeout and creating the page for connecting with wifi
GyverPortal ui;         // UI interface
Temperature dataT;

// display information about temperature on SSD1306 display
void displayOnScreenTask(void *pvParameters)
{
  // Setup
  Screen *screen = (Screen *)pvParameters;
  screen->initialize(); // Inițializarea ecranului
  Serial.println(WiFi.localIP());

  // Loop
  for (;;)
  {
    // Afișați textul dorit pe ecran
    screen->clearDisplay();
    screen->printText(30, 0, "Temperature");
    // Serial.println(dataT.readNTCTemp());
    screen->printText(0, 20, "O:" + String(dataT.readNTCTemp()));

    float ktemp;
    dataT.readKTemp(&ktemp); // Pasăm adresa variabilei ktemp pentru a fi actualizată de funcția readKTemp

    screen->printText(60, 20, "I:" + String(ktemp));
    screen->printText(40, 40, "S:" + String(dataT.set_Temp));

    vTaskDelay(2000 / portTICK_PERIOD_MS); // Afișați textul timp de 2 secunde
  }
}

// For creating a graph on WEB interface is used thta tasck
void displayGraphOnWebsite(void *pvParameters)
{
  (void)pvParameters;
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
  // Setup
  (void)pvParameters;

  initControl(dataT);

  // Loop
  for (;;)
  {
    // get temperature and set PWM
    compute(dataT);

    vTaskDelay(PERIOD_VALUE / portTICK_PERIOD_MS);
  }
}

// build website page
void build()
{

  GP.BUILD_BEGIN(700);
  GP.THEME(GP_LIGHT);

  GP.TITLE("Temperature control");
  GP.HR();

  // Home,Graph,List is the pahe in the website
  GP.NAV_TABS_LINKS("/,/Graph,/List", "Home,Graph,List");

  // second WINDOW Graph
  if (ui.uri() == "/Graph")
  {
    GP.PLOT_STOCK_DARK<2, NUMBER_OF_TEMP_VALUE>("table", dataT.names, dataT.dates, dataT.temp);

    M_BLOCK_TAB(
        "View",
        M_BOX(GP.LABEL("Real temperature: "); GP.LABEL(String(dataT.K_Temp), "val"););
        M_BOX(GP.LABEL("Set temperature"); GP.NUMBER("num", "Set value", dataT.set_Temp););
        M_BOX(GP.BUTTON("btn1", "Start"); GP.BUTTON("btn2", "Stop");););
  }
  // last WINDOW List
  else if (ui.uri() == "/List")
  {
    GP.BLOCK_BEGIN(GP_TAB, "", "Log", GP_DEFAULT);
    GP.BUTTON("btn3", "Print");
    GP.AREA("list", 20, dataT.list);
  }
  // first WINDOW Home
  else
  {
    GP_MAKE_GRID(
        M_BLOCK_TAB(
            "View",
            M_BOX(GP.LABEL("Real temperature: "); GP.LABEL(String(dataT.K_Temp), "val"););
            M_BOX(GP.LABEL("Set temperature"); GP.NUMBER("num", "Set value", dataT.set_Temp););
            M_BOX(GP.BUTTON("btn1", "Start"); GP.BUTTON("btn2", "Stop");););

        M_BLOCK_TAB(
            "PID CONFIG",
            M_BOX(GP.LABEL("Kp"); GP.SPINNER("kp", dataT.Kp, 0, 40, 0.1, 1); GP.LABEL("Recommended 0.1"););
            M_BOX(GP.LABEL("Ki"); GP.SPINNER("ki", dataT.Ki, 0, 40, 0.05, 1); GP.LABEL("Recommended 0.05"););
            M_BOX(GP.LABEL("Kd"); GP.SPINNER("kd", dataT.Kd, 0, 40, 0.01, 1); GP.LABEL("Recommended 0.01"););););
  }
  GP.BLOCK_END();
}

void action(void)
{
  float KP, KI, KD;
  // if component was prest
  if (ui.click())
  {
    // check component and update value
    if (ui.clickInt("num", dataT.set_Temp))
    {
      Serial.print("Number: ");
      Serial.println(dataT.set_Temp);
    }

    if (ui.click("btn1"))
    { // start
      dataT.flag[START_FLAG_INDEX] = true;

      Serial.println("Start");
    }
    if (ui.click("btn2"))
    {
      dataT.flag[START_FLAG_INDEX] = false;

      Serial.println("Stop");
    }
    if (ui.click("btn3"))
    {
      ui.updateString("list", dataT.list);
    }
    if (ui.copyFloat("kp", KP))
      dataT.Kp = KP;
    if (ui.copyFloat("ki", KI))
      dataT.Ki = KI;
    if (ui.copyFloat("kd", KD))
      dataT.Kd = KD;
  }
}

void setup()
{
  Serial.begin(115200);

  xTaskCreatePinnedToCore(displayOnScreenTask, "Display On Screen Task", 1000, new Screen(), 0, &DisplayTextOnScreenTasck, 0);

  xTaskCreatePinnedToCore(displayGraphOnWebsite, "Display Graph on webpage Task", 1000, NULL, 1, &DisplayGraphUITask, 1);
  // // Funcția pentru task-ul de afișare // Numele task-ului// Dimensiunea stivei (modificați la nevoie) // Parametrii pentru task (obiectul Screen)// Prioritate (cu o valoare mai mare, task-ul rulează mai repede)
  xTaskCreatePinnedToCore(PIDControl, "Implement PID algorithm", 1000, NULL, 1, &ControlTemperatureTask, 0);

  vTaskStartScheduler();

  // conetct and start site info
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void loop()
{
  ps.checkConfigPortalRequest(); // if button is presed than start reconnecting to new wifi
  ui.tick();
  vTaskDelay(200 / portTICK_PERIOD_MS);
}