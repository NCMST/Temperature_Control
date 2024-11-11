#include <Arduino.h>
#include <WiFiUdp.h>
#include <Adafruit_I2CDevice.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#include "unity.h"
#include "temperature.hpp"

Temperature dataT;

const float MIN_TEMP = -50.0;
const float MAX_TEMP = 800.0;

void setUp(void)
{
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("Setup completed. Starting tests...");
}

void tearDown(void)
{
    Serial.println("Tearing down after test...");
}

void test_function_readNTCTemp(void)
{
    float temp = dataT.readNTCTemp();
    Serial.print("NTC Temperature Read: ");
    Serial.println(temp);

    TEST_ASSERT_NOT_EQUAL(NAN, temp);
    TEST_ASSERT_GREATER_THAN(MIN_TEMP, temp);
    TEST_ASSERT_LESS_THAN(MAX_TEMP, temp);
}

void test_function_readKTemp(void)
{
    float temp = dataT.readKTemp();
    Serial.print("K Temperature Read: ");
    Serial.println(temp);

    TEST_ASSERT_NOT_EQUAL(NAN, temp);
    TEST_ASSERT_GREATER_THAN(MIN_TEMP, temp);
    TEST_ASSERT_LESS_THAN(MAX_TEMP, temp);
}

void test_function_pid_values(void)
{
    dataT.setKp(0.2);
    dataT.setKi(0.1);
    dataT.setKd(0.05);

    TEST_ASSERT_EQUAL_FLOAT(0.2, dataT.getKp());
    TEST_ASSERT_EQUAL_FLOAT(0.1, dataT.getKi());
    TEST_ASSERT_EQUAL_FLOAT(0.05, dataT.getKd());
}

void test_function_pid_result(void)
{
    dataT.setLimit(MIN_PID_VALUE, MAX_PID_VALUE);

    dataT.setSetpoint(100); 
    dataT.setInput(90);     

    float result = dataT.getResult();

    Serial.print("PID Result: ");
    Serial.println(result);
    
    TEST_ASSERT_GREATER_THAN(MIN_PID_VALUE - 1, result);
    TEST_ASSERT_LESS_THAN(MAX_PID_VALUE + 1, result);
}

void runAllTests(void)
{
    UNITY_BEGIN();
    
    RUN_TEST(test_function_readNTCTemp);
    RUN_TEST(test_function_readKTemp);
    
    // PID
    RUN_TEST(test_function_pid_values);
    RUN_TEST(test_function_pid_result);

    UNITY_END();
}

void setup()
{
    delay(2000); 
    runAllTests();
}

void loop() {}