#include "WebServerManager.hpp"

// Constructorul clasei
WebServerManager::WebServerManager(const char *ssid, const char *password, const char *secon, const char *seconPass)
    : ssid(ssid), password(password), server(HTTP_PORT), second_ssid(secon), second_password(seconPass)
{
    currentTemperature.startFlag = false;

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    homePage = readFile("/home.html");

    graphPage = readFile("/graph.html");
}

String WebServerManager::readFile(const char *path)
{
    String content;

    File file = SPIFFS.open(path, "r");

    if (!file || file.isDirectory())
    {
        Serial.println("Failed to open file for reading");
        return String();
    }

    while (file.available())
    {
        content += String((char)file.read());
    }

    file.close();

    return content;
}

/**
 * @brief start the web server, return 1 if is not connected to the wifi in 10 seconds
 *
 * @details if is not connected to the wifi in 10 seconds return 1, use setupWiFIRouter to chenge the mode to router or setWiFiCredentials to change the credentials
 *
 * @return int
 */
int WebServerManager::begin()
{
    Serial.begin(115200);

    // Adaugă SSID-urile la WiFiMulti
    wifiMulti.addAP(ssid, password);               // Prima rețea
    wifiMulti.addAP(second_ssid, second_password); // A doua rețea

    unsigned long startAttemptTime = millis();

    // Începe să încerce conectarea la rețelele Wi-Fi
    while (wifiMulti.run() != WL_CONNECTED)
    {
        if (millis() - startAttemptTime >= TEN_SEC) // Verifică dacă au trecut 10 secunde
        {
            if (LOGS_MESSAGE)
                Serial.println("Error: Failed to connect to WiFi");

            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            return 1; // Eșec la conectare
        }
        vTaskDelay(100); // Așteaptă 0.1 secundă înainte de a încerca din nou
        if (LOGS_MESSAGE)
            Serial.println("Conectare la WiFi...");
    }

    if (LOGS_MESSAGE)
    {
        Serial.println("Conectat la WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }

    server.on("/", [this]()
              { handleHome(); }); // Setează ruta principală
    server.on("/graph", [this]()
              { handleGraph(); }); // Setează ruta pentru grafic
    server.on("/temperature", [this]()
              { handleTemperatureData(); }); // Setează ruta pentru datele temperaturii

    server.on("/setpoint", HTTP_POST, [this]()
              { handleSetPoint(); });

    server.on("/command", HTTP_POST, [this]()
              { handleCommand(); });

    server.begin(); // Pornește serverul

    return 0;
}

/**
 * @brief Seting up the router
 *
 * @param ssid
 * @param password
 */
void WebServerManager::setupWiFIRouter(const char *ssid, const char *password)
{
    WiFi.mode(WIFI_AP);
    if (WiFi.softAP(ssid, password)) {
        Serial.print("Access Point \"");
        Serial.print(ssid);
        Serial.println("\" started.");
        
        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);
        
        // Configurează rutele serverului web pentru modul AP
        server.on("/", [this]() { handleHome(); });
        server.on("/graph", [this]() { handleGraph(); });
        server.on("/temperature", [this]() { handleTemperatureData(); });
        
        server.begin(); // Pornește serverul
    } else {
        Serial.println("Failed to start Access Point.");
    }
}

void WebServerManager::handleCommand()
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (!error)
    {
        String action = doc["action"];
        if (action == "start")
        {

            currentTemperature.startFlag = true;
            // Handle start command
            if (LOGS_MESSAGE)
                Serial.println("Start command received.");
            // Add your start logic here
        }
        else if (action == "stop")
        {

            currentTemperature.startFlag = false;
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

void WebServerManager::handleSetPoint()
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (!error)
    {
        float setpoint = doc["setpoint_temperature"];
        int workingTime = doc["working_time"]; // Get working time from JSON

        stetTemperature = setpoint;
        currentTemperature.setTime = workingTime;

        currentTemperature.setpoint_temperature = setpoint; // Update your temperature data structure
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
// Funcția de gestionare a cererilor pentru pagina principală
void WebServerManager::handleHome()
{
    server.send(200, "text/html", homePage);
}

// Funcția de gestionare a cererilor pentru pagina graficului
void WebServerManager::handleGraph()
{
    server.send(200, "text/html", graphPage);
}

// Funcția de gestionare a cererilor pentru datele temperaturii
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

// Funcția care primește temperatura dintr-un task
void WebServerManager::setTemperatureData(const TemperatureData &tempData)
{
    currentTemperature = tempData; // Stochează datele primite în variabila curentă.
}

// Funcția de gestionare a cererilor clientului
void WebServerManager::handleClient()
{
    server.handleClient(); // Gestionează cererile clientului.
}
