#include "ESP32WiFiConfig.h"

ESP32WiFiConfig::ESP32WiFiConfig(const char* ap_ssid, const char* ap_password, int buttonPin, WebServer& server)
    : ap_ssid(ap_ssid), ap_password(ap_password), buttonPin(buttonPin), server(server), shouldStartAP(false) {}

/**
 * @brief setup
 * 
 * @details Setup WiFi
 * @note Should be called after WiFi.begin
 */
void ESP32WiFiConfig::setup() {
    Serial.begin(115200);
    EEPROM.begin(512);
    pinMode(buttonPin, INPUT_PULLUP);

    if (digitalRead(buttonPin) == LOW) {
        shouldStartAP = true;
    } else {
        String ssid = readEEPROM(0);
        String password = readEEPROM(32);

        if (ssid.length() > 0 && password.length() > 0) {
            WiFi.begin(ssid.c_str(), password.c_str());

            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 20) {
                delay(500);
                Serial.print(".");
                attempts++;
            }

            if (WiFi.status() != WL_CONNECTED) {
                shouldStartAP = true;
            }
        } else {
            shouldStartAP = true;
        }
    }

    if (shouldStartAP) {
        startAP();
    } else {
        Serial.println("\nConnected to WiFi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
}

/**
 * @brief loop
 * 
 * @details Check if Access Point should be started
 */
void ESP32WiFiConfig::loop() {
    if (shouldStartAP) {
        server.handleClient();
    }
}

/**
 * @brief startAP
 * 
 * @details Start Access Point
 */
void ESP32WiFiConfig::startAP() {
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println("Access Point Started");
    Serial.print("SSID: ");
    Serial.println(ap_ssid);
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", [this]() { handleRoot(); });
    server.on("/save", [this]() { handleSave(); });
    server.begin();
}

/**
 * @brief handleRoot
 * 
 * @details Send HTML form
 */
void ESP32WiFiConfig::handleRoot() {
    String html = "<html><body style=\"display:flex;flex-direction:column;align-items:center;justify-content:center;margin:10px;\">";
    html += "<div style=\"display:flex;flex-direction:column;align-items:center;justify-content:center;\">";
    html += "<h1 style=\"text-align:center;\">ESP32 WiFi Configuration</h1>";
    html += "<h1 style=\"text-align:center;\">Configurare WiFi ESP32</h1>";
    html += "</div>";
    html += "<form action='/save' method='POST' style=\"display:flex;flex-direction:column;align-items:center;justify-content:center;\">";
    html += "SSID: <input type='text' name='ssid' style=\"width: 200px;\"><br>";
    html += "Password: <input type='password' name='password' style=\"width: 200px;\"><br>";
    html += "<input type='submit' value='Save'>";
    html += "</form></body></html>";
    server.send(200, "text/html", html);
}

/**
 * @brief handleSave
 * 
 * @details Save data to EEPROM
 * 
 */
void ESP32WiFiConfig::handleSave() {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    writeEEPROM(0, ssid);
    writeEEPROM(32, password);
    EEPROM.commit();

    server.send(200, "text/plain", "Credentials saved. ESP32 will reconnect.");
    delay(1000);
    ESP.restart();
}

/**
 * @brief writeEEPROM
 * 
 * @param startAddr 
 * @param data 
 * 
 * @return void
 * 
 * @details Write data to EEPROM
 */
void ESP32WiFiConfig::writeEEPROM(int startAddr, String data) {
    for (int i = 0; i < data.length(); i++) {
        EEPROM.write(startAddr + i, data[i]);
    }
    EEPROM.write(startAddr + data.length(), '\0');
}

/**
 * @brief readEEPROM
 * 
 * @param startAddr 
 * @return String 
 * 
 * @details Read data from EEPROM
 */
String ESP32WiFiConfig::readEEPROM(int startAddr) {
    String data = "";
    char c;
    int i = 0;
    while ((c = EEPROM.read(startAddr + i)) != '\0' && i < 32) {
        data += c;
        i++;
    }
    return data;
}