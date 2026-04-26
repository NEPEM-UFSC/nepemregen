#pragma once

#include <stdint.h>

class IrrigationCtrl {
public:
  static void init();
  static void process(float current_soil_moisture, float current_temp,
                      float current_hum);

private:
  static const float SETPOINT_CRITICAL;
  static const uint32_t T_PULSE_MS;
  static const uint32_t T_LAG_MS;

  static uint32_t last_pulse_time;
  static bool in_cooldown;
};
