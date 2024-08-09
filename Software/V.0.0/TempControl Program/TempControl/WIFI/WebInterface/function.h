#pragma once

#ifndef FUNCTION_H
#define FUNCTION_H

#include "esp32-hal-gpio.h"
#include "config.h"
#include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd(0x27, 16, 2);
// extern
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COL, LCD_ROW);

class Display {
private:

public:
  bool init() {
    lcd.init();
    lcd.backlight();
    return 0;
  }
  void bitMap(void) {
    lcd.setCursor(0, 0);
    lcd.print("Real:");

    lcd.setCursor(9, 0);
    lcd.print("Tim:");

    lcd.setCursor(0, 1);
    lcd.print("Set:");
  }
  // sendIP(WiFi.localIP())
  void printIP(IPAddress IP) {
    lcd.setCursor(7, 0);
    lcd.print("IP");
    lcd.setCursor(1, 1);
    lcd.print(IP);
    delay(5000);
    lcd.clear();
  }
  void printTemp(int temp) {
    lcd.setCursor(6, 0);
    lcd.print(temp);
  }
  void printTime(long int time) {
    lcd.setCursor(14, 0);
    lcd.print(time);
  }
  void printSetTemp(long int temp) {
    lcd.setCursor(6, 1);
    lcd.print(temp);
    lcd.setCursor(10, 1);
    lcd.print("Celsius");
  }
  void update(void) {
    lcd.clear();
    this->bitMap();
  }
};

typedef struct {
  uint8_t bit_map_flag, wifi_connect_flag, start_heating_flag;
} Flag;

typedef struct {
  int16_t arr[2][NUMBER_OF_TEMP_VALUE];  // first index real temp second seted
  int actualReal, actualSet, lastReal, lastSet, counter;
} Value;

typedef struct {
  uint64_t startMillis, relativTime1, relativTime2;
} Time;

typedef struct Pid {
  double kp, kd, ki, fil, fil2;
};

const char* names[] = {
  "Real_temp", "Set_temp"
};
uint32_t dates[NUMBER_OF_TEMP_VALUE];

GyverPortal ui;

Flag flag{ .bit_map_flag = 0, .wifi_connect_flag = 0, .start_heating_flag = 0 };
Value val{ .actualReal = 0, .actualSet = 0 };
Time times{ .startMillis = 0 };
Display display;

Pid data;

String list = String("real, seted, time");

void setPin(void) {
  // Led and
  pinMode(PWM_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  //  for read position
  pinMode(ZD_PIN, INPUT_PULLUP);
  // analogSetWidth(11);  // 11Bit resolution
  // analogSetAttenuation(ADC_0db);
  // set pin
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
}

// build website page
void build() {

  GP.BUILD_BEGIN(700);
  GP.THEME(GP_LIGHT);

  GP.TITLE("Temperature control");
  GP.HR();

  GP.NAV_TABS_LINKS("/,/Graph,/List", "Home,Graph,List");
  // second WINDOW
  if (ui.uri() == "/Graph") {
    GP.PLOT_STOCK_DARK<2, NUMBER_OF_TEMP_VALUE>("table", names, dates, val.arr);

    M_BLOCK_TAB(
      "View",
      M_BOX(GP.LABEL("Real temperature: "); GP.LABEL(String(val.actualReal), "val"););
      M_BOX(GP.LABEL("Set temperature"); GP.NUMBER("num", "Set value", val.actualSet););
      M_BOX(GP.BUTTON("btn1", "Start"); GP.BUTTON("btn2", "Stop");););

  } else if (ui.uri() == "/List") {
    // last WINDOW
    GP.BLOCK_BEGIN(GP_TAB, "", "Log", GP_DEFAULT);
    GP.BUTTON("btn3", "Print");
    GP.AREA("list", 20, list);

  } else {
    // first WINDOW

    GP_MAKE_GRID(
      M_BLOCK_TAB(
        "View",
        M_BOX(GP.LABEL("Real temperature: "); GP.LABEL(String(val.actualReal), "val"););
        M_BOX(GP.LABEL("Set temperature"); GP.NUMBER("num", "Set value", val.actualSet););
        M_BOX(GP.BUTTON("btn1", "Start"); GP.BUTTON("btn2", "Stop");););

      M_BLOCK_TAB(
        "PID CONFIG",
        GP_MAKE_BOX(GP.LABEL("Kp"); GP.SPINNER("kp", data.kp, 0, 40, 0.1, 1););
        GP_MAKE_BOX(GP.LABEL("Ki"); GP.SPINNER("ki", data.ki, 0, 40, 0.1, 1););
        GP_MAKE_BOX(GP.LABEL("Filt q."); GP.SLIDER("fil", data.fil, 0, 1, 0.1););
        GP_MAKE_BOX(GP.LABEL("Filt er."); GP.SLIDER("fil2", data.fil2, 0, 100, 1););););
  }
  GP.BLOCK_END();
}


void action(void) {

  // if component was prest
  if (ui.click()) {
    // check component and update value
    if (ui.clickInt("num", val.actualSet)) {
      Serial.print("Number: ");
      Serial.println(val.actualSet);
    }

    if (ui.click("btn1")) {  //start
      flag.start_heating_flag = 1;

      digitalWrite(RED_LED_PIN, HIGH);

      Serial.println("Start");
    }
    if (ui.click("btn2")) {
      flag.start_heating_flag = 0;

      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);

      Serial.println("Stop");
    }
    if (ui.click("btn3")) {
      ui.updateString("list", list);
    }
  }
}

#endif  // FUNCTION_Hs