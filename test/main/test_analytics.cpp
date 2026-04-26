#include "edge_analytics.hpp"
#include "unity.h"
#include <cmath>

void test_ema_filter_basic(void) {
  edge_analytics::EMAFilter filter(0.5f);

  // First value should set s_t
  TEST_ASSERT_EQUAL_FLOAT(10.0f, filter.filter(10.0f));

  // Second value: 0.5 * 20.0 + 0.5 * 10.0 = 15.0
  TEST_ASSERT_EQUAL_FLOAT(15.0f, filter.filter(20.0f));

  // Third value: 0.5 * 30.0 + 0.5 * 15.0 = 22.5
  TEST_ASSERT_EQUAL_FLOAT(22.5f, filter.filter(30.0f));
}

void test_calculate_vpd(void) {
  // Known values for VPD calculation
  // Temp: 25C, Hum: 60%
  // es = 0.6108 * exp((17.27 * 25) / (25 + 237.3)) ~= 3.167 kPa
  // ea = 3.167 * 0.6 = 1.900 kPa
  // vpd = 3.167 - 1.900 = 1.267 kPa
  float vpd = edge_analytics::calculate_vpd(25.0f, 60.0f);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.267f, vpd);
}

void test_calculate_dew_point(void) {
  // Known values for Dew Point
  // Temp: 25C, Hum: 60% -> Dew point should be around 16.7C
  float td = edge_analytics::calculate_dew_point(25.0f, 60.0f);
  TEST_ASSERT_FLOAT_WITHIN(0.5f, 16.7f, td);
}

void test_gdd_accumulation(void) {
  edge_analytics::reset_gdd();
  TEST_ASSERT_EQUAL_FLOAT(0.0f, edge_analytics::get_accumulated_gdd());

  // Day 1: Tmax=30, Tmin=20, Tbase=10 -> GDD = ((30+20)/2) - 10 = 15
  edge_analytics::update_gdd(20.0f, 10.0f);
  edge_analytics::update_gdd(30.0f, 10.0f);
  edge_analytics::commit_daily_gdd(10.0f);

  TEST_ASSERT_EQUAL_FLOAT(15.0f, edge_analytics::get_accumulated_gdd());

  // Day 2: Tmax=25, Tmin=15, Tbase=10 -> GDD = ((25+15)/2) - 10 = 10
  edge_analytics::update_gdd(15.0f, 10.0f);
  edge_analytics::update_gdd(25.0f, 10.0f);
  edge_analytics::commit_daily_gdd(10.0f);

  TEST_ASSERT_EQUAL_FLOAT(25.0f, edge_analytics::get_accumulated_gdd());
}
