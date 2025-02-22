/**
 * @file test_pid.cpp
 * @author Creciune Catalin
 * @brief Test functions for PID class
 * @version 0.1
 * @date 2025-02-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <unity.h>
#include "pid.hpp"

void setUp(void) {
    
}

void tearDown(void) {
    
}

/**
 * @brief Tests the PID controller with output limits.
 * 
 * This function creates a PID controller with specific gains and sets output limits.
 * It then tests the PID controller with input values that exceed the set limits to 
 * ensure the output is correctly limited.
 * 
 * The PID controller is initialized with the following gains:
 * - Proportional gain (Kp): 1.0
 * - Integral gain (Ki): 0.1
 * - Derivative gain (Kd): 0.05
 * 
 * The output limits are set as follows:
 * - Minimum limit: 0.0
 * - Maximum limit: 10.0
 * 
 * The function performs the following tests:
 * - Computes the PID output for an input value of 20.0 with a setpoint of 5.0.
 *   The expected output should be limited to 10.0.
 * - Computes the PID output for an input value of -20.0 with a setpoint of 5.0.
 *   The expected output should be limited to 0.0.
 * 
 * The tests use the TEST_ASSERT_FLOAT_WITHIN macro to check if the computed output
 * is within 0.01 of the expected limited value.
 */
void test_pid_with_limits() {
    PID pid(1.0, 0.1, 0.05);
    
    pid.setLimits(0.0, 10.0); 

    // Test with values that exceed the limits
    TEST_ASSERT_FLOAT_WITHIN(0.01, 10.0, pid.compute(20.0, 5.0)); // Should be limited to 10.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, pid.compute(-20.0, 5.0)); // Should be limited to 0.0
}

/**
 * @brief 
 * 
 */
void test_pid_output(){
    PID pid(1.0, 0.1, 0.05);
    pid.setLimits(0.0, 10.0);

    TEST_ASSERT_FLOAT_WITHIN(0.01, 5.0, pid.compute(5.0, 5.0)); // No error, output should be 5.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 6.0, pid.compute(6.0, 5.0)); // Error of 1.0, output should be 6.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 4.0, pid.compute(4.0, 5.0)); // Error of -1.0, output should be 4.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 7.0, pid.compute(7.0, 5.0)); // Error of 2.0, output should be 7.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.0, pid.compute(3.0, 5.0)); // Error of -2.0, output should be 3.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 8.0, pid.compute(8.0, 5.0)); // Error of 3.0, output should be 8.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, pid.compute(2.0, 5.0)); // Error of -3.0, output should be 2.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 9.0, pid.compute(9.0, 5.0)); // Error of 4.0, output should be 9.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1.0, pid.compute(1.0, 5.0)); // Error of -4.0, output should be 1.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 10.0, pid.compute(10.0, 5.0)); // Error of 5.0, output should be 10.0 (limited to max)
}

/**
 * @brief Main function
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_pid_with_limits);
    RUN_TEST(test_pid_output);
    return UNITY_END();
}
