#pragma once

#include <stdint.h>

class VentilationCtrl {
public:
  static void init();
  static void process(float current_temp, float current_hum,
                      float current_soil_moisture, bool is_business_hours);

private:
  static const uint32_t WINDOW_30M_MS;
  static const uint32_t TARGET_5M_MS;
  static const float TEMP_MAX;
  static const float HUM_MAX;

  static uint32_t window_start_time;
  static uint32_t accumulated_fan_time;
  static uint32_t fan_turn_on_time;
};
