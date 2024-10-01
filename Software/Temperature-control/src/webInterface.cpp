#include "webInterface.h"
#include "GyverPortal.h"

GyverPortal ui;
extern Data dataT; // Asigură-te că ai definit extern Data dataT în alt fișier

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

void action() {
    float KP, KI, KD;

    if (ui.click()) {
        if (ui.clickInt("num", dataT.set_Temp)) {
            Serial.print("Number: ");
            Serial.println(dataT.set_Temp);
        }

        if (ui.click("btn1")) { // start
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