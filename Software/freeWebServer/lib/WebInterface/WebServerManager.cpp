/**
 * @file WebServerManager.cpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief WebServerManager class implementation
 * @version 0.1
 * @date 2025-02-21
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "WebServerManager.hpp"

/**
 * @brief Construct a new Web Server Manager:: Web Server Manager object
 *
 * @param ssid
 * @param password
 * @param secon
 * @param seconPass
 */
WebServerManager::WebServerManager(const char *ssid, const char *password, const char *secon, const char *seconPass)
    : ssid(ssid), password(password), server(HTTP_PORT), second_ssid(secon), second_password(seconPass)
{

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    homePage = fileManager.readFile("/home.html");

    graphPage = fileManager.readFile("/graph.html");

    listPage = fileManager.readFile("/list.html");

    if (LOGS_MESSAGE)
    {
        fileManager.listSPIFFSFiles();
        Serial.println(listPage);
    }
}

/**
 * @brief start the web server, return 1 if not connected to the wifi in 10 seconds
 *
 * @details if not connected to the wifi in 10 seconds return 1, use setupWiFIRouter to change the mode to router or setWiFiCredentials to change the credentials
 *
 * @return int
 */
int WebServerManager::begin()
{
    Serial.begin(115200);

    // Add SSIDs to WiFiMulti
    wifiMulti.addAP(ssid, password);               // First network
    wifiMulti.addAP(second_ssid, second_password); // Second network

    unsigned long startAttemptTime = millis();

    // Start trying to connect to Wi-Fi networks
    while (wifiMulti.run() != WL_CONNECTED)
    {
        if (millis() - startAttemptTime >= TIME_BEFOR_AP) // Check if 10 seconds have passed
        {
            if (LOGS_MESSAGE)
                Serial.println("Error: Failed to connect to WiFi");

            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            return 1; // Connection failed
        }
        vTaskDelay(100); // Wait 0.1 second before trying again
        if (LOGS_MESSAGE)
            Serial.println("Connecting to WiFi...");
    }

    if (LOGS_MESSAGE)
    {
        Serial.println("Connected to WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }

    server.on("/", [this]()
              { handleHome(); }); // Set the main route
    server.on("/graph", [this]()
              { handleGraph(); }); // Set the route for the graph

    server.on("/list", [this]()
              { handleList(); }); // Set the route for the list

    server.on("/temperature", [this]()
              { handleTemperatureData(); }); // Set the route for temperature data

    server.on("/setpoint", HTTP_POST, [this]()
              { handleSetPoint(); });

    server.on("/command", HTTP_POST, [this]()
              { handleCommand(); });

    server.on("/download", [this]()
              { handleDownload(); }); // Set the route for download

    server.on("/pid", HTTP_POST, [this]()
              { handlePID(); }); // Set the route for PID

    server.on("/pid_constants", HTTP_GET, [this]()
              { handlePIDPrint(); }); // Set the route for PID constants

    server.begin(); // Start the server

    return 0;
}

/**
 * @brief Setting up the router
 *
 * @param ssid
 * @param password
 */
void WebServerManager::setupWiFIRouter(const char *ssid, const char *password)
{
    WiFi.mode(WIFI_AP);
    if (WiFi.softAP(ssid, password))
    {
        Serial.print("Access Point \"");
        Serial.print(ssid);
        Serial.println("\" started.");

        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);

        // Configure web server routes for AP mode
        server.on("/", [this]()
                  { handleHome(); });
        server.on("/graph", [this]()
                  { handleGraph(); });
        server.on("/temperature", [this]()
                  { handleTemperatureData(); });

        server.begin(); // Start the server
    }
    else
    {
        Serial.println("Failed to start Access Point.");
    }
}

/**
 * @brief handle the command
 *
 * as of now, the command can be start or stop
 */
void WebServerManager::handleCommand()
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (!error)
    {
        String action = doc["action"];
        if (action == "start")
        {

            startFlag = true;

            // Handle start command
            if (LOGS_MESSAGE)
                Serial.println("Start command received.");
            // Add your start logic here
        }
        else if (action == "stop")
        {

            startFlag = false;

            // Handle stop command
            if (LOGS_MESSAGE)
                Serial.println("Stop command received.");
            // Add your stop logic here
        }
        server.send(200, "text/plain", "Command received");
    }
    else
    {
        if (LOGS_MESSAGE)
            Serial.println("Failed to parse JSON");
        server.send(400, "text/plain", "Invalid JSON");
    }
}

/**
 * @brief handle the set point
 */
void WebServerManager::handleSetPoint()
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (!error)
    {
        float setpoint = doc["setpoint_temperature"];
        uint32_t workingTime = doc["working_time"]; // Get working time from JSON

        stetTemperature = setpoint;
        setTime = workingTime;

        // currentTemperature.setpoint_temperature = setpoint; // Update your temperature data structure
        Serial.print("Set point updated to: ");
        Serial.println(setpoint);

        Serial.print("Working time set to: ");
        Serial.println(workingTime); // You can use this value as needed

        server.send(200, "text/plain", "Set point updated"); // Send a response back
    }
    else
    {
        Serial.println("Failed to parse JSON");
        server.send(400, "text/plain", "Invalid JSON");
    }
}

/**
 * @brief handle the home page
 */
void WebServerManager::handleHome()
{
    server.send(200, "text/html", homePage);
}

/**
 * @brief handle the graph page
 */
void WebServerManager::handleGraph()
{
    server.send(200, "text/html", graphPage);
}

/**
 * @brief handle the list page
 *
 */
void WebServerManager::handleList()
{
    server.send(200, "text/html", listPage);
}

/**
 * @brief handle the temperature data
 */
void WebServerManager::handleTemperatureData()
{
    JsonDocument doc;

    doc["inside_temperature"] = currentTemperature.inside_temperature;
    doc["outside_temperature"] = currentTemperature.outside_temperature;
    doc["setpoint_temperature"] = currentTemperature.setpoint_temperature;

    String jsonResponse;
    serializeJson(doc, jsonResponse);

    server.send(200, "application/json", jsonResponse);
}

/**
 * @brief handle the temperature list
 *
 */
void WebServerManager::handleTemperatureList()
{
    JsonDocument doc;

    JsonArray array = doc.to<JsonArray>();

    for (const auto &entry : temperatureHistory)
    {
        JsonObject obj = array.add<JsonObject>();
        obj["timestamp"] = entry.timestamp;
        obj["inside_temperature"] = entry.inside_temperature;
        obj["outside_temperature"] = entry.outside_temperature;
    }

    String jsonResponse;
    serializeJson(doc, jsonResponse);

    server.send(200, "application/json", jsonResponse);
}

/**
 * @brief set the temperature data
 *
 * @param tempData
 */
void WebServerManager::setTemperatureData(const TemperatureData &tempData)
{
    currentTemperature = tempData; // Store the received data in the current variable.

    stetTemperature = tempData.setpoint_temperature;
    startFlag = tempData.startFlag;
    setTime = tempData.setTime;
}

/**
 * @brief handle the client
 *
 */
void WebServerManager::handleClient()
{
    server.handleClient(); // Handle client requests.
    // handleCommand();
}

/**
 * @brief download the list.csv file
 *
 */
void WebServerManager::handleDownload()
{
    // get the file
    File file = SPIFFS.open("/list.csv", "r");
    if (!file)
    {
        server.send(404, "text/plain", "File not found");
        return;
    }

    server.streamFile(file, "text/csv");
    file.close();
}

/**
 * @brief helper function to list the files in the SPIFFS
 *
 * PID constants can be updated using this endpoint
 */
void WebServerManager::handlePID()
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (!error)
    {
        float kp = doc["kp"];
        float ki = doc["ki"];
        float kd = doc["kd"];

        if (LOGS_MESSAGE)
        {
            Serial.print("PID constants updated: ");
            Serial.print("Kp=");
            Serial.print(kp);
            Serial.print(", Ki=");
            Serial.print(ki);
            Serial.print(", Kd=");
            Serial.println(kd);
        }

        server.send(200, "text/plain", "PID constants updated");
    }
    else
    {
        if (LOGS_MESSAGE)
            Serial.println("Failed to parse JSON");
        server.send(400, "text/plain", "Invalid JSON");
    }
}

/**
 * @brief handle the PID constants
 *
 */
void WebServerManager::handlePIDPrint()
{
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["kp"] = kp;
    jsonDoc["ki"] = ki;
    jsonDoc["kd"] = kd;

    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);
    server.send(200, "application/json", jsonResponse);
}

/**
 * @brief update the list.csv file
 *
 * @param realTemperature
 * @param setTemperature
 * @param time
 */
void WebServerManager::updateCSV(float realTemperature, float setTemperature, uint32_t time)
{
    String data = String(realTemperature) + "," + String(setTemperature) + "," + String(time) + "\n";

    if (!fileManager.updateFile("/list.csv", data.c_str()))
    {
        if (LOGS_MESSAGE)
            Serial.println("Failed to open file for writing");
        return;
    }
}
