// #include "unity.h"
// #include "screen.hpp"

// Screen screen;

// void setUp(void) {
//   Serial.begin(115200);
//   while (!Serial);
//   Screen screen = Screen();
// }

// void tearDown(void) {
//   delete &Serial;
//   delete &screen;
// }

// void test_function_init_screen(void) {
//   // test stuff
//   TEST_ASSERT_EQUAL(true, screen.init());
// }

// void test_function_printText_screen(void) {
//   // more test stuff
//   TEST_ASSERT_EQUAL(true, screen.printText(0, 0, 1, "test"));
// }

// int runUnityTests(void) {
//   UNITY_BEGIN();
//   RUN_TEST(test_function_init_screen);
//   RUN_TEST(test_function_printText_screen);
//   return UNITY_END();
// }


// void setup() {
//   // Wait ~2 seconds before the Unity test runner
//   // establishes connection with a board Serial interface
//   delay(2000);

//   runUnityTests();
// }
// void loop() {}
