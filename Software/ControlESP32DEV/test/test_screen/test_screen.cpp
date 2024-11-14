#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <ESPmDNS.h> 
#include <Arduino.h>
#include <GyverPortal.h>
#include <Update.h>
#include "unity.h"
#include "screen.hpp"

Screen screen;

void setUp(void) {
    Serial.begin(115200);
    while (!Serial); // Așteptați conectarea la serial
}

void tearDown(void) {
    // Nu este necesar să ștergeți Serial sau screen
}

void test_function_init_screen(void) {
    TEST_ASSERT_EQUAL(true, screen.init());
}

void test_function_printText_screen(void) {
    TEST_ASSERT_EQUAL(true, screen.printText(0, 0, 1, "test"));
}

void test_function_printText_invalid(void) {
    // Testați un caz invalid
    TEST_ASSERT_EQUAL(false, screen.printText(-1, -1, 1, "test")); // Exemplu: coordonate invalide
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_function_init_screen);
    RUN_TEST(test_function_printText_screen);
    RUN_TEST(test_function_printText_invalid); // Adăugați teste suplimentare
    return UNITY_END();
}

void setup() {
    delay(2000); // Așteptați inițializarea
    runUnityTests();
}

void loop() {}