/**
 * @file WebServerManager.hpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef WEBSERVERMANAGER_HPP
#define WEBSERVERMANAGER_HPP

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <vector>
#include <FS.h>
#include <SPIFFS.h>
#include "TemperatureData.hpp"

/**
 * @brief HTTP port
 * 
 */
#define HTTP_PORT 80

/**
 * @brief log messages to the serial monitor
 * 
 * boolean value
 * 
 */
#define LOGS_MESSAGE true

/**
 * @brief Time before the access point is started
 * 
 * if the connection to the Wi-Fi network fails
 * esp32 will start an access point 
 */
#define TIME_BEFOR_AP 5000

/**
 * @brief LED_BUILTIN pin
 * 
 */
#define LED_BUILTIN 2

class WebServerManager 
/**
 * @class WebServerManager
 * @brief Class for managing the web server
 * 
 * @details Loads the HTML pages, handles the requests, and sends the responses.
 * This class manages the connection to Wi-Fi networks and serves web pages to clients.
 * It also handles temperature data and provides various endpoints for interacting with the web server.
 * 
 * @public 
 * @param ssid - SSID of the primary Wi-Fi network
 * @param password - Password of the primary Wi-Fi network
 * @param second_ssid - SSID of the secondary Wi-Fi network
 * @param second_password - Password of the secondary Wi-Fi network
 * 
 * @fn begin - Initializes the web server and starts listening for client requests.
 * @fn handleHome - Handles requests to the home page.
 * @fn handleList - Handles requests to the list page.
 * @fn handleGraph - Handles requests to the graph page.
 * @fn handleTemperatureData - Handles requests for temperature data.
 * @fn handleTemperatureList - Handles requests for the list of temperature data.
 * @fn handleClient - Handles client connections.
 * @fn setTemperatureData - Sets the current temperature data.
 * @fn handleSetPoint - Handles requests to set the temperature set point.
 * @fn handleCommand - Handles various commands sent to the server.
 * @fn getTemperature - Returns the current temperature data.
 * @fn getStetTemperature - Returns the set temperature.
 * @fn getStartFlag - Returns the start flag status.
 * @fn getSetTime - Returns the set time.
 * @fn setWiFiCredentials - Sets the Wi-Fi credentials for the primary network.
 * @fn setupWiFIRouter - Sets up the Wi-Fi router with the given credentials.
 * @fn handleDownload - Handles requests to download data.
 * @fn updateCSV - Updates the CSV file with temperature data.
 * 
 * @private 
 * @var ssid - SSID of the primary Wi-Fi network.
 * @var password - Password of the primary Wi-Fi network.
 * @var second_ssid - SSID of the secondary Wi-Fi network.
 * @var second_password - Password of the secondary Wi-Fi network.
 * @var server - Web server object.
 * @var homePage - HTML content of the home page.
 * @var graphPage - HTML content of the graph page.
 * @var listPage - HTML content of the list page.
 * @var stetTemperature - Set temperature value.
 * @var startFlag - Flag indicating whether the server has started.
 * @var setTime - Time set for the operation.
 * @var temperatureHistory - History of temperature data.
 * @var currentTemperature - Current temperature data.
 * @var wifiMulti - Object for managing multiple Wi-Fi networks.
 */
{
public:
    void setWiFiCredentials(const char* ssid, const char* password){
        this->ssid = ssid;
        this->password = password;
    }

    void setupWiFIRouter(const char* ssid, const char* password);

    void handleDownload();

    void updateCSV(float realTemperature, float setTemperature, uint32_t time);

private:
    const char* ssid;         // SSID of the Wi-Fi network
    const char* password;     // Password of the Wi-Fi network
    const char* second_ssid;  // SSID of the second Wi-Fi network
    const char* second_password; // Password of the second Wi-Fi network
    WebServer server;         // Web server object
    String homePage;          // HTML home page
    String graphPage;         // HTML graph page
    String listPage;

    float stetTemperature;
    bool startFlag;
    uint32_t setTime;
    
    std::vector<TemperatureData> temperatureHistory;

    TemperatureData currentTemperature; // Variable for storing the current temperature

    WiFiMulti wifiMulti; // Object for managing multiple Wi-Fi networks
};

#endif // WEBSERVERMANAGER_HPP
