#include "personalserver.h"
#include <functional>

PersonalServer::PersonalServer(uint8_t timeout = 120)
{
    pinMode(pinResetWifi, INPUT_PULLUP); // pin is conetet to 3.3V

    bool forceConfig = false;

    bool spiffsSetup = loadConfigFile();

    if (!spiffsSetup)
    {
        Serial.println(F("Forcing config mode as there is no saved config"));
        forceConfig = true;
    }

    WiFi.mode(WIFI_STA); // WiFi access point

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    resetSettings();

    setSaveConfigCallback(std::bind(&PersonalServer::saveConfigCallback, this));

    setAPCallback(std::bind(&PersonalServer::configModelCallback, this, std::placeholders::_1));

    // Text box (String) - 50 characters maximum
    WiFiManagerParameter custom_text_box("key_text", "Enter your string here", testString, 50);

    // Need to convert numerical input to string to display the default value.
    char convertedValue[6];
    sprintf(convertedValue, "%d", testNumber);

    // Text box (Number) - 7 characters maximum
    WiFiManagerParameter custom_text_box_num("key_num", "Enter your number here", convertedValue, 7);

    // Add all defined parameters
    addParameter(&custom_text_box);
    addParameter(&custom_text_box_num);

    if (forceConfig)
    // Run if we need a configuration
    {
        if (!startConfigPortal("NEWTEST_AP", "password"))
        {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
            // reset and try again, or maybe put it to deep sleep
            ESP.restart();
            delay(5000);
        }
    }
    else
    {
        if (!autoConnect("NEWTEST_AP", "password"))
        {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
            // if we still have not connected restart and try all over again
            ESP.restart();
            delay(5000);
        }
    }

    // If we get here, we are connected to the WiFi

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Lets deal with the user config values

    // Copy the string value
    strncpy(testString, custom_text_box.getValue(), sizeof(testString));
    Serial.print("testString: ");
    Serial.println(testString);

    // Convert the number value
    testNumber = atoi(custom_text_box_num.getValue());
    Serial.print("testNumber: ");
    Serial.println(testNumber);

    // Save the custom parameters to FS
    if (shouldSaveConfig)
    {
        saveConfigFile();
    }
}

void PersonalServer::saveConfigFile()
{
    StaticJsonDocument<512> json;

    strcpy(testString, "test string hello");
    testNumber = 123;

    json["string"] = testString;
    json["number"] = testNumber;

    File confiFile = SPIFFS.open(JSON_CONFIG_FILE, "w");

    if (!confiFile)
    {
        Serial.println("Failed to open file");
    }
    confiFile.close();
}

int PersonalServer::loadConfigFile()
{
    if (SPIFFS.begin(false) || SPIFFS.begin(true))
    {
        if (SPIFFS.exists(JSON_CONFIG_FILE))
        {
            File confiFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
            if (confiFile)
            {
                StaticJsonDocument<512> json;
                DeserializationError error = deserializeJson(json, confiFile);

                serializeJsonPretty(json, Serial);

                if (!error)
                {
                    strcpy(testString, json["string"]);
                    testNumber = json["number"].as<int>();

                    return true;
                }
                else
                {
                    Serial.println("Failed to load json config");
                }
            }
        }
    }
    else
    {
        Serial.println("Failed to mount FS");
    }

    return false;
}

using saveConfigCallback = std::function<void()>;

void PersonalServer::saveConfigCallback()
{
    // We need to save configuration
    shouldSaveConfig = true;
}

using configModelCallback = std::function<void(WiFiManager *)>;

void PersonalServer::configModelCallback(WiFiManager *myWiFiManager)
{
    Serial.println(myWiFiManager->getConfigPortalSSID());
    Serial.println(WiFi.softAPIP());
}

void PersonalServer::checkConfigPortalRequest()
{
    if (digitalRead(pinResetWifi) == LOW)
    {
        startConfigPortal("OnDemandAP");
    }
}
