#include "unity.h"
#include <iostream>

// Prototypes from test_analytics.cpp
void test_ema_filter_basic(void);
void test_calculate_vpd(void);
void test_calculate_dew_point(void);
void test_gdd_accumulation(void);
void test_irrigation_logic_critical(void);
void test_nvs_persistence_after_reset(void);

// For Unity on Host
void setUp(void) {}
void tearDown(void) {}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_ema_filter_basic);
  RUN_TEST(test_calculate_vpd);
  RUN_TEST(test_calculate_dew_point);
  RUN_TEST(test_gdd_accumulation);
  RUN_TEST(test_irrigation_logic_critical);
  RUN_TEST(test_nvs_persistence_after_reset);
  return UNITY_END();
}
