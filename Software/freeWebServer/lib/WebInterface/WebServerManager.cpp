#include "WebServerManager.hpp"

// Constructorul clasei
WebServerManager::WebServerManager(const char *ssid, const char *password)
    : ssid(ssid), password(password), server(HTTP_PORT)
{

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

// Funcția de inițializare a serverului și conectare la Wi-Fi
void WebServerManager::begin()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password); // Conectează-te la Wi-Fi

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(1000);
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
    // server.on("/list", [this]()
    //           { handleTemperatureList(); }); // Setează ruta pentru datele temperaturii in lista

    server.on("/setpoint", HTTP_POST, [this]()
              { handleSetPoint(); });

    server.begin(); // Pornește serverul
}

void WebServerManager::handleSetPoint()
{
    if (server.method() == HTTP_POST)
    {
        String body = server.arg("plain");        // Get the raw body of the request
        Serial.println("Received body: " + body); // Debug log

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, body); // Parse JSON from request body

        if (error)
        {
            Serial.print("Failed to parse JSON: ");
            Serial.println(error.f_str());
            server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid JSON\"}");
            return;
        }

        if (doc.containsKey("setpoint_temperature"))
        {
            stetTemperature = doc["setpoint_temperature"];

            if (LOGS_MESSAGE)
            {
                Serial.print("Setpoint temperature updated to: ");
                Serial.println(stetTemperature);
                currentTemperature.setpoint_temperature = stetTemperature;
            }

            server.send(200, "application/json", "{\"status\":\"success\"}");
        }
        else
        {
            server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid data\"}");
        }
    }
    else
    {
        server.send(405, "text/plain", "Method Not Allowed");
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
void WebServerManager::handleTemperatureData() {
    DynamicJsonDocument doc(1024);
    doc["inside_temperature"] = currentTemperature.inside_temperature;
    doc["outside_temperature"] = currentTemperature.outside_temperature;
    doc["setpoint_temperature"] = currentTemperature.setpoint_temperature;

    String jsonResponse;
    serializeJson(doc, jsonResponse);

    server.send(200, "application/json", jsonResponse);
}

void WebServerManager::handleTemperatureList()
{
    DynamicJsonDocument doc(1024);

    JsonArray array = doc.to<JsonArray>();

    for (const auto &entry : temperatureHistory)
    {
        JsonObject obj = array.createNestedObject();
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