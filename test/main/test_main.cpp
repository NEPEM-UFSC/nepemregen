#include "unity.h"
#include "esp_system.h"
#include "esp_log.h"

// Protótipos de testes aqui
// void test_example(void) { TEST_ASSERT_EQUAL(1, 1); }

extern "C" void app_main(void)
{
    UNITY_BEGIN();
    // RUN_TEST(test_example);
    UNITY_END();
}
