#pragma once
// for ESP32 NODEMCU
#ifndef SCREENMETHODS_H
#define SCREENMETHODS_H

#include "config.h"
#include "esp32-hal-gpio.h"
#include <GyverMAX6675.h>
#include "GyverPID.h"
#include <GyverPortal.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

GyverPortal ui;
GyverMAX6675< CLK_PIN, DATA_PIN, CS_PIN > sens;

class Data : public GyverPID {
public:
  float K_Temp, NTC_Temp, set_Temp, Last_K_Temp, Last_NTC_Temp, Last_set_Temp;
  int16_t temp[2][NUMBER_OF_TEMP_VALUE];
  const char* names[2];
  String list;
  uint32_t dates[NUMBER_OF_TEMP_VALUE];
  uint8_t flag[START_FLAG_INDEX + 1];
  Data()
    : GyverPID() {
    names[0] = "Real_temp";
    names[0] = "Set_temp";
    Last_K_Temp = Last_NTC_Temp = Last_set_Temp = NTC_Temp = set_Temp = K_Temp = 0;
    list = String("real, seted, time");
    dates[NUMBER_OF_TEMP_VALUE - 1] = GPunix(2023, 10, 9, 0, 0, 0, 0);
    Kd = Ki = Kp = 0;
  }

  float readKTemp() {
    if (sens.readTemp()) {
      K_Temp = sens.getTemp();
      return K_Temp;
    }
  }
  float readNTCTemp() {
    double Vout, Rth, temperature, adc_value;

    adc_value = ADC_RESOLUTION - analogRead(NTC_PIN) + 0.5;  // switch direction
    Vout = (adc_value * VCC) / ADC_RESOLUTION;
    Rth = (VCC * NTC_R2 / Vout) - NTC_R2;  // Formula for R2 as Pull-down: Vcc-Rth-R2-GND
    /*  Steinhart-Hart Thermistor Equation:
 *  Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
 */
    temperature = (1 / (NTC_A + (NTC_B * log(Rth)) + (NTC_C * pow((log(Rth)), 3))));
    return temperature - 273.15;
  }
};

extern Data dataT;

class Screen : public Adafruit_SSD1306 {
public:
  Screen(int8_t reset_pin = OLED_RESET)
    : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, reset_pin) {}

  void initialize() {
    if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("Eroare - SSD1306 nu a fost găsit."));
      while (1)
        ;  // Blochează programul în caz de eroare
    }
    display();       // Afișează ecranul inițial gol
    delay(2000);     // Așteptați 2 secunde
    clearDisplay();  // Ștergeți ecranul pentru a pregăti pentru text
  }

  void printText(uint8_t x, uint8_t y, uint8_t size, String text) {
    // clearDisplay();  // Ștergeți ecranul pentru a afișa text nou
    setTextColor(SSD1306_WHITE);
    setTextSize(1);
    setCursor(x, y);
    println(text);
    display();
  }
};

// build website page
void build() {

  GP.BUILD_BEGIN(700);
  GP.THEME(GP_LIGHT);

  GP.TITLE("Temperature control");
  GP.HR();

  GP.NAV_TABS_LINKS("/,/Graph,/List", "Home,Graph,List");
  // second WINDOW
  if (ui.uri() == "/Graph") {
    GP.PLOT_STOCK_DARK<2, NUMBER_OF_TEMP_VALUE>("table", dataT.names, dataT.dates, dataT.temp);

    M_BLOCK_TAB(
      "View",
      M_BOX(GP.LABEL("Real temperature: "); GP.LABEL(String(dataT.K_Temp), "val"););
      M_BOX(GP.LABEL("Set temperature"); GP.NUMBER("num", "Set value", dataT.set_Temp););
      M_BOX(GP.BUTTON("btn1", "Start"); GP.BUTTON("btn2", "Stop");););

  } else if (ui.uri() == "/List") {
    // last WINDOW
    GP.BLOCK_BEGIN(GP_TAB, "", "Log", GP_DEFAULT);
    GP.BUTTON("btn3", "Print");
    GP.AREA("list", 20, dataT.list);

  } else {
    // first WINDOW

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

void action(void) {
  float KP, KI, KD;
  // if component was prest
  if (ui.click()) {
    // check component and update value
    if (ui.clickInt("num", dataT.set_Temp)) {
      Serial.print("Number: ");
      Serial.println(dataT.set_Temp);
    }

    if (ui.click("btn1")) {  //start
      dataT.flag[START_FLAG_INDEX] = true;

      Serial.println("Start");
    }
    if (ui.click("btn2")) {
      dataT.flag[START_FLAG_INDEX] = false;

      Serial.println("Stop");
    }
    if (ui.click("btn3")) {
      ui.updateString("list", dataT.list);
    }
    if (ui.copyFloat("kp", KP)) dataT.Kp = KP;
    if (ui.copyFloat("ki", KI)) dataT.Ki = KI;
    if (ui.copyFloat("kd", KD)) dataT.Kd = KD;
  }
}

#endif  // CONFIG_H