#include "config.h"
#include "function.h"

TaskHandle_t Task0, Task1;

GyverMAX6675< CLK_PIN, DATA_PIN, CS_PIN > sens;

extern Display display;
extern GyverPortal ui;
extern Flag flag;
extern Value val;
extern const char* names[];

extern Time times;
extern uint32_t dates[];

void core0(void* pvParameters) {
  // ==========SETUP===========
  // init UART with 115200 bout rate
  Serial.begin(115200);
  // init pin
  setPin();
  // init displaz
  display.init();
  vTaskDelay(100 / portTICK_PERIOD_MS);
  // print IP
  if (flag.wifi_connect_flag) {
    display.printIP(WiFi.localIP());
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }

  val.lastReal = 0;
  val.lastSet = 0;

  val.actualReal = 0;
  val.actualSet = 0;
  // ============LOOP===========
  for (;;) {

    if (!flag.bit_map_flag) {
      display.bitMap();
      flag.bit_map_flag = 1;
    }

    if (sens.readTemp()) {

      val.actualReal = sens.getTempInt();

      // if (val.actualReal != val.lastReal || val.actualSet != val.lastSet) {

      display.update();
      display.printTemp(val.actualReal);
      display.printSetTemp(val.actualSet);


      val.lastSet = val.actualSet;
      val.lastReal = val.actualReal;
      // }
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void displayGraph(void* pvParameters) {
  val.counter = 0;
  // ============LOOP===========
  for (;;) {
    if (flag.start_heating_flag) {
      val.counter++;

      GPaddInt(val.actualReal, val.arr[0], NUMBER_OF_TEMP_VALUE);
      GPaddInt(val.actualSet, val.arr[1], NUMBER_OF_TEMP_VALUE);
      GPaddUnixS(5, dates, NUMBER_OF_TEMP_VALUE);

      list += "\n" + String(val.actualReal) + ", " + String(val.actualSet);
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
   
  }
}


void setup() {
  // init WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }
  // initialize the lcd
  flag.wifi_connect_flag = 1;

  // init core 0
  xTaskCreatePinnedToCore(core0, "Task0", 10000, NULL, 1, &Task0, 0);
  xTaskCreatePinnedToCore(displayGraph, "Task1", 10000, NULL, 1, &Task1, 0);

  //  init graph date
  dates[NUMBER_OF_TEMP_VALUE - 1] = GPunix(2023, 10, 9, 0, 0, 0, 0);

  // conetct and start site info
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void loop() {
  ui.tick();

  vTaskDelay(1 / portTICK_PERIOD_MS);
}