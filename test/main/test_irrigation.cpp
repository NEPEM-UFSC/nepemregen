#include "hal_gpio.h"
#include "irrigation_ctrl.h"
#include "time_utils.h"
#include "unity.h"

void test_irrigation_logic_critical(void) {
  // Setup
  IrrigationCtrl::init();
  HalGpio::init();
  HalGpio::reset_history();

  // Initial state
  TEST_ASSERT_FALSE(HalGpio::is_pump_on());
  TEST_ASSERT_FALSE(HalGpio::was_pump_ever_on);

  // Process with safe moisture (60% > 50%)
  IrrigationCtrl::process(60.0f, 25.0f, 60.0f);
  TEST_ASSERT_FALSE(HalGpio::is_pump_on());
  TEST_ASSERT_FALSE(HalGpio::was_pump_ever_on);

  // Process with critical moisture (40% < 50%)
  IrrigationCtrl::process(40.0f, 25.0f, 60.0f);

  // In simulation, vTaskDelay is noop, so pump turns ON and then OFF
  // immediately. We check if it WAS on.
  TEST_ASSERT_TRUE(HalGpio::was_pump_ever_on);
  TEST_ASSERT_FALSE(HalGpio::is_pump_on()); // Should be OFF now
}

// Simple history record for mocks
static bool pump_was_turned_on = false;

// We could refactor hal_gpio to support a "spy" or just use a flag
// For now, let's just check the state after process.
// If vTaskDelay is no-op, the pump will be OFF after process().
// But we can check if it WAS on if we modify the mock.
