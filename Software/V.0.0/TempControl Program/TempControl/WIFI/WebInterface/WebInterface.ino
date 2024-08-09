#include "config.h"
#include "function.h"
#include "ctime"

TaskHandle_t Task0, Task1, Task2;

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
    Serial.println(WiFi.localIP());

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

      display.update();
      display.printTemp(val.actualReal);
      // Serial.println(val.actualReal);
      display.printSetTemp(val.actualSet);


      val.lastSet = val.actualSet;
      val.lastReal = val.actualReal;
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

void setPower(void* pvParameters) {
  // times.startMillis = millis();
  Serial.begin(1000000);
  // init pin
  setPin();
  for (;;) {
    // if (millis() - times.startMillis > 1000) {
    //   Serial.println("Pha  ; " + String(val.counter));
    //   val.counter = 0;
    // }
    // if (!digitalRead(ZD_PIN)) {
    Serial.println(String(analogRead(ZD_PIN)));
    // }
    vTaskDelay(pdMS_TO_TICKS(2));
  }
}

// int dimmer;               // переменная диммера

// void IRAM_ATTR isr() {
//   Serial.println("Detect");
//   // static int lastDim;
//   // digitalWrite(DIMMER_PIN, 0);  // выключаем симистор
//   // если значение изменилось, устанавливаем новый период
//   // если нет, то просто перезапускаем со старым
//   // if (lastDim != dimmer) Timer2.setPeriod(lastDim = dimmer);
//   // else Timer2.restart();
// }

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
  // xTaskCreatePinnedToCore(core0, "Task0", 10000, NULL, 1, &Task0, 0);
  // xTaskCreatePinnedToCore(displayGraph, "Task1", 10000, NULL, 1, &Task1, 0);
  xTaskCreatePinnedToCore(setPower, "Task2", 2048, NULL, 1, &Task2, 0);
  // attachInterrupt(ZD_PIN, isr, FALLING);  // для самодельной схемы ставь FALLING

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