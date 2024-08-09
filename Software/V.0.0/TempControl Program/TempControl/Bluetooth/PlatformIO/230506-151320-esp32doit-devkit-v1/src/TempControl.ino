#include <Arduino.h>
#include <LCD_I2C.h>
#include "BluetoothSerial.h"
#include <GyverMAX6675.h>
#include "GyverPID.h"
// #include <stdio.h>

#if !defined(CONFIG_BT_ENABLED)
|| !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled !Please run ` make menuconfig ` to and enable it
#endif

// Pinii modulului MAX6675K
#define CLK_PIN 14
#define DATA_PIN 12
#define CS_PIN 15

// LCD PIN
#define SDA_PIN 21
#define SCL_PIN 22

// pin de control
#define PWM_PIN 16
// pini de indicare
#define GREEN_LED_PIN 18 // setet temperature is now
#define RED_LED_PIN 19   // work but temperature noi is like setet
// time interval
#define interval 5000
#define interval2 1000

        void
        readCOM(void);
void readBT(void);
void setGetTemp(void);
void setupBluetooth(void);
void bitMap(void);

unsigned long previousMillis = 0;
unsigned long previousMillis2;
unsigned long startMillis = 0;
int tT;
int temp;
uint8_t startFlag = 0;
uint8_t startFlagPrint = 0;
uint8_t startFlagTime = 0;

LCD_I2C lcd(0x27, 16, 2);
GyverPID regulator(0.1, 0.05, 0.01, 20);
BluetoothSerial SerialBT;
GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> sens;

void setup()
{
  lcd.begin();
  lcd.backlight();
  delay(500);
  bitMap();

  Serial.begin(115200);

  setupBluetooth();

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
  readCOM();
  readBT();
  setGetTemp();
}

void setGetTemp(void)
{
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
    digitalWrite(PWM_PIN, (int)regulator.getResultTimer());
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
    if ((temp != 0 and temp < 1000) and (tT > -1 and tT < 1000))
    {
      if (startFlagPrint)
      {

        temperatureString = String(temp) + " " + String(tT) + " " + String((int)regulator.getResultTimer()) + " " + String((millis() - startMillis) / 60000);

        SerialBT.println(temperatureString);
        Serial.println(temperatureString);

        lcd.setCursor(5, 0);
        lcd.print(String(temp) + (char)223);

        lcd.setCursor(13, 0);
        lcd.print(String((millis() - startMillis) / 60000));

        lcd.setCursor(5, 1);
        lcd.print(String(tT) + (char)223);
      }

      else
      {
        // Real temp, Set temp
        temperatureString = String(temp) + " " + String(tT);
        SerialBT.println(temperatureString);
        Serial.println(temperatureString);

        lcd.setCursor(5, 0);
        lcd.print(String(temp) + (char)223);

        lcd.setCursor(5, 1);
        lcd.print(String(tT) + (char)223);
      }
    }
  }
}

void readBT(void)
{
  if (SerialBT.available() > 1)
  {
    char key = SerialBT.read();
    int val = SerialBT.parseInt();

    switch (key)
    {
    case 't':
      tT = val;
      regulator.setpoint = tT;
      break;
    case 's':
      startFlag = val;
      lcd.setCursor(13, 0);
      lcd.print(String("   "));
      break;
    default:
      break;
    }
  }
}

void readCOM(void)
{
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
      lcd.print(String("   "));
      break;
    default:
      break;
    }
  }
}

void setupBluetooth(void)
{
  SerialBT.begin("ESP32test"); // Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void bitMap(void)
{
  lcd.setCursor(0, 0);
  lcd.print("Real:");

  lcd.setCursor(9, 0);
  lcd.print("Tim:");

  lcd.setCursor(0, 1);
  lcd.print("Set:");
}
