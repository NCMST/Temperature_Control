#include <unity.h>
#include "pid.hpp"

void setUp(void) {
    // Inițializări necesare înainte de fiecare test (dacă este cazul)
}

void tearDown(void) {
    // Curățiri necesare după fiecare test (dacă este cazul)
}

void test_pid_with_limits() {
    PID pid(1.0, 0.1, 0.05);
    
    pid.setLimits(0.0, 10.0); // Setează limitele

    // Testează cu valori care depășesc limitele
    TEST_ASSERT_FLOAT_WITHIN(0.01, 10.0, pid.compute(20.0, 5.0)); // Ar trebui să fie limitat la 10.0
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, pid.compute(-20.0, 5.0)); // Ar trebui să fie limitat la 0.0
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_pid_with_limits);
    return UNITY_END();
}