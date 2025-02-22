// /**
//  * @file test_screen.cpp
//  * @author Creciunel Catalin
//  * @brief test functions for screen class
//  * @version 0.1
//  * @date 2025-02-22
//  * 
//  * @copyright Copyright (c) 2025
//  * 
//  */
// #include "unity.h"
// #include "screen.hpp"

// Screen screen;

// /**
//  * @brief Set the Up object
//  * 
//  * seting up the serial communication
//  */
// void setUp(void) {
//   Serial.begin(115200);
//   while (!Serial);
//   Screen screen = Screen();
// }

// /**
//  * @brief Tear down the object
//  * 
//  * deleting the serial communication
//  */
// void tearDown(void) {
//   delete &Serial;
//   delete &screen;
// }

// /**
//  * @brief Test function for init screen
//  * 
//  */
// void test_function_init_screen(void) {
//   // test stuff
//   TEST_ASSERT_EQUAL(true, screen.init());
// }

// /**
//  * @brief Test function for printText screen
//  * 
//  */
// void test_function_printText_screen(void) {
//   // more test stuff
//   TEST_ASSERT_EQUAL(true, screen.printText(0, 0, 1, "test"));
// }

// /**
//  * @brief Run the Unity test suite
//  * 
//  * @return int 
//  */
// int runUnityTests(void) {
//   UNITY_BEGIN();
//   RUN_TEST(test_function_init_screen);
//   RUN_TEST(test_function_printText_screen);
//   return UNITY_END();
// }

// /**
//  * @brief Setup the Arduino environment
//  * 
//  */
// void setup() {
//   // Wait ~2 seconds before the Unity test runner
//   // establishes connection with a board Serial interface
//   delay(2000);

//   runUnityTests();
// }

// void loop() {}
// // 