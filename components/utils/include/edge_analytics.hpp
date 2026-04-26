#pragma once

#include "esp_attr.h"
#include <math.h>

namespace edge_analytics {

// 1. Exponential Moving Average (EMA)
class EMAFilter {
public:
  EMAFilter(float alpha);
  float filter(float current_value);
  void reset(float initial_value);

private:
  float alpha;
  float s_t;
  bool initialized;
};

// 2. Thermodynamics
float calculate_vpd(float temp_c, float humidity_rh);
float calculate_dew_point(float temp_c, float humidity_rh);

// 3. Growing Degree Days (GDD)
// To be called periodically with current temperature.
// Tbase is the base temperature for the specific plant (e.g., 10.0 C)
void update_gdd(float current_temp, float t_base);
void commit_daily_gdd(float t_base);
void update_gdd_fractional(float current_temp, float t_base,
                           float delta_t_days);
float get_accumulated_gdd();
void reset_gdd();

} // namespace edge_analytics
