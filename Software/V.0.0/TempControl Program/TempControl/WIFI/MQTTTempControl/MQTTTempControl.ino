
#include <PubSubClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecure.h>

#include <GyverMAX6675.h>
#include "GyverPID.h"
#include <ArduinoJson.h>

// Pinii modulului MAX6675K
#define CLK_PIN 14
#define DATA_PIN 12
#define CS_PIN 15

//LCD PIN
#define SDA_PIN 21
#define SCL_PIN 22

// pin de control
#define PWM_PIN 16
// pini de indicare
#define GREEN_LED_PIN 18 // setet temperature is now
#define RED_LED_PIN 19 // work but temperature noi is like setet
// time interval
#define interval 10000
#define interval2 1000

void readBT(void);
void setGetTemp(void);
void bitMap(void);
void reconnect(void);
void setupWifi(void);
void callback(char* topic, byte* payload, unsigned int length);
void publishMessage (const char* topic, String payload, boolean retained);

unsigned long previousMillis = 0;
unsigned long previousMillis2;
unsigned long startMillis = 0;
int tT;
int temp;
uint8_t startFlag = 0;
uint8_t startFlagPrint = 0;
uint8_t startFlagTime = 0;

const char* ssid = "ICG";
const char* password = "engineering704";

const char* mqttServer = "cd37b5646cb048a88b67278686a7e52c.s2.eu.hivemq.cloud";
//IPAddress server("cd37b5646cb048a88b67278686a7e52c.s2.eu.hivemq.cloud");
const char* mqttUserName = "hivemq.webclient.1683105584004";
const char* mqttPassword = "FdP#4reIp@N>5t02lQ!V";
const int mqttPort = 8884;

LiquidCrystal_I2C lcd(0x27, 16, 2);
GyverPID regulator(0.1, 0.05, 0.01, 20);
GyverMAX6675 < CLK_PIN, DATA_PIN, CS_PIN > sens;

//connect WIFI
WiFiClientSecure espClient;

PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);
  
  setupWifi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  lcd.init();                      // initialize the lcd
  lcd.backlight();
  bitMap();

  regulator.setDirection(NORMAL); // de jos-sus, Reverso sus-jos
  regulator.setLimits(0, 1); // limita de valori pe output, max 255 de la PWM
  regulator.setpoint = tT; // temperatura reala
  // pid constante
  regulator.Kp = 240;
  regulator.Ki += 19;
  regulator.Kd = 600;

  pinMode(PWM_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  previousMillis2 = previousMillis = millis();
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
}

void loop()
{
  if (!client.connected())reconnect();
  client.loop();
}

void setGetTemp(void) {
  if (millis() - previousMillis2 >= interval2)
  {
    if (sens.readTemp())
    {
      temp = sens.getTempInt();
    }
    previousMillis2 = millis();
  }
  regulator.input = temp;
  if (startFlag)
  {
    digitalWrite(PWM_PIN, (int) regulator.getResultTimer());
    startFlagPrint = 1;
    if (!startFlagTime)
    {
      startMillis = millis();
      startFlagTime = 1;
    }
    if (temp - tT <= 3 && temp - tT >= -3)
    {
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
    }
    else
    {
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
    }
  }
  else
  {
    startFlagPrint = 0;
    startFlagTime = 0;
    digitalWrite(PWM_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
  String temperatureString = "";
  if (millis() - previousMillis >= interval)
  {
    previousMillis = millis();
    if (temp != 0 and temp < 1000)
    {
      if (startFlagPrint)
      {
        // Real temp, Set temp, PID val, time in minute / 60000
        temperatureString = String(temp) + " " + String(tT) + " " + String((int) regulator.getResultTimer()) + " " + String((millis() - startMillis) / 60000);
        Serial.println(temperatureString);
        lcd.setCursor(5, 0);
        lcd.print(String(temp) + (char)223);

        lcd.setCursor(13, 0);
        lcd.print( String((millis() - startMillis) / 60000));

        lcd.setCursor(5, 1);
        lcd.print(String(tT) + (char)223);
      }
      else
      {
        // Real temp, Set temp
        temperatureString = String(temp) + " " + String(tT);
        Serial.println(temperatureString);

        lcd.setCursor(5, 0);
        lcd.print( String(temp) + (char)223);

        lcd.setCursor(5, 1);
        lcd.print(String(tT) + (char)223);
      }
    }
  }
}

void readBT(void) {
  if (Serial.available() > 1)
  {
    char key = Serial.read();
    int val = Serial.parseInt();
    switch (key)
    {
      case 't':
        tT = val;
        regulator.setpoint = tT;
        break;
      case 's':
        startFlag = val;
        lcd.setCursor(13, 0);
        lcd.print( String("   "));
        break;
      default:
        break;
    }
  }
}

void bitMap(void) {
  lcd.setCursor(0, 0);
  lcd.print("Real:");

  lcd.setCursor(9, 0);
  lcd.print("Tim:");

  lcd.setCursor(0, 1);
  lcd.print("Set:");
}

void reconnect(void) {
  while (!client.connected()) {
    Serial.print("Asteapta MQTT conectarea ...");
    String clientID = "ESP32Client-";
    clientID += String(random(0xffff), HEX);

    if (client.connect(clientID.c_str(), mqttUserName, mqttPassword)) {
      Serial.println("conectet");
      client.subscribe("#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" 5 sec");
      delay(5000);
    }
  }
}

void setupWifi(void) {
  Serial.print("\nConnect to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  randomSeed(micros());
  Serial.println("\nWIF connected\n IP address:");
  Serial.println(WiFi.localIP());
}

/***** Call back Method for Receiving MQTT messages and Switching LED ****/
void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage += (char)payload[i];
  Serial.println("Message arrived [" + String(topic) + "]" + incommingMessage);
  //--- check the incomming message
  if (strcmp(topic, "#") == 1) {
    if (incommingMessage.equals("1")) digitalWrite(GREEN_LED_PIN, HIGH); // Turn the LED on
    else digitalWrite(GREEN_LED_PIN, LOW); // Turn the LED off
  }
}

/**** Method for Publishing MQTT Messages **********/
void publishMessage (const char* topic, String payload, boolean retained) {
  if (client.publish (topic, payload.c_str(), true))
    Serial.println("Message publised [" + String (topic) + "]: " + payload);
}
