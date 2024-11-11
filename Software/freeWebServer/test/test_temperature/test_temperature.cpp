#include <unity.h>
#include "temperature.hpp"
#include "MockTemperatureSensor.hpp"

void setUp(void) {}

void tearDown(void) {}

void test_readKTemp_with_valid_data() {
    MockTemperatureSensor mock(25.0f, true); // Temperatura simulată de 25°C, citire validă
    Temperature temp(&mock);
    
    float result = temp.readKTemp();
    
    TEST_ASSERT_FLOAT_WITHIN(0.01, 25.0f, result); // Verifică dacă rezultatul este corect
}

void test_readKTemp_with_invalid_data() {
    MockTemperatureSensor mock(0.0f, false); // Citire invalidă
    Temperature temp(&mock);
    
    float result = temp.readKTemp();
    
    TEST_ASSERT_TRUE(isnan(result)); // Verifică dacă rezultatul este NAN
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_readKTemp_with_valid_data);
    RUN_TEST(test_readKTemp_with_invalid_data);
    
    return UNITY_END();
}