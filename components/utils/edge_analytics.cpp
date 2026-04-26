#include "edge_analytics.hpp"
#include <algorithm>
#include <cmath>
#include <esp_attr.h>

namespace edge_analytics {

// --- EMA Filter ---
EMAFilter::EMAFilter(float alpha)
    : alpha(alpha), s_t(0.0f), initialized(false) {}

float EMAFilter::filter(float current_value) {
  if (!initialized) {
    s_t = current_value;
    initialized = true;
  } else {
    s_t = alpha * current_value + (1.0f - alpha) * s_t;
  }
  return s_t;
}

void EMAFilter::reset(float initial_value) {
  s_t = initial_value;
  initialized = true;
}

// --- Thermodynamics ---
float calculate_vpd(float temp_c, float humidity_rh) {
  // es = 0.6108 * exp((17.27 * temp) / (temp + 237.3))
  float es = 0.6108f * std::exp((17.27f * temp_c) / (temp_c + 237.3f));
  // ea = es * (umidade / 100.0)
  float ea = es * (humidity_rh / 100.0f);
  return es - ea; // Retorna em kPa
}

float calculate_dew_point(float temp_c, float humidity_rh) {
  // Magnus-Tetens formula approximation
  float gamma =
      (17.27f * temp_c) / (237.3f + temp_c) + std::log(humidity_rh / 100.0f);
  float td = (237.3f * gamma) / (17.27f - gamma);
  return td;
}

// --- GDD Tracking ---
RTC_DATA_ATTR float rtc_accumulated_gdd = 0.0f;
RTC_DATA_ATTR float rtc_daily_tmax = -273.15f;
RTC_DATA_ATTR float rtc_daily_tmin = 1000.0f;
RTC_DATA_ATTR bool rtc_gdd_initialized = false;

// In a real application, you would call calculate_daily_gdd once per day
// or sum fractional GDDs. For this example, we'll provide a fractional update
// assuming it's called periodically, or a daily update if passing min/max
// directly.

// Fractional GDD approach (Riemann sum) if called periodically with delta_time
// in days: This avoids needing precise daily boundaries if we just integrate
// continuously. However, the standard formula is daily.

// Let's implement a fractional integration.
// If called e.g., every 1 hour (delta_t_days = 1.0/24.0)
void update_gdd_fractional(float current_temp, float t_base,
                           float delta_t_days) {
  if (current_temp > t_base) {
    rtc_accumulated_gdd += (current_temp - t_base) * delta_t_days;
  }
}

// Standard daily calculation (user requested)
void update_gdd(float current_temp, float t_base) {
  if (!rtc_gdd_initialized) {
    rtc_daily_tmax = current_temp;
    rtc_daily_tmin = current_temp;
    rtc_gdd_initialized = true;
  } else {
    rtc_daily_tmax = std::max(rtc_daily_tmax, current_temp);
    rtc_daily_tmin = std::min(rtc_daily_tmin, current_temp);
  }
}

void commit_daily_gdd(float t_base) {
  if (rtc_gdd_initialized) {
    float gdd_today = ((rtc_daily_tmax + rtc_daily_tmin) / 2.0f) - t_base;
    if (gdd_today > 0) {
      rtc_accumulated_gdd += gdd_today;
    }
    // Reset for next day
    rtc_gdd_initialized = false;
    rtc_daily_tmax = -273.15f;
    rtc_daily_tmin = 1000.0f;
  }
}

float get_accumulated_gdd() { return rtc_accumulated_gdd; }

void reset_gdd() {
  rtc_accumulated_gdd = 0.0f;
  rtc_gdd_initialized = false;
}

} // namespace edge_analytics
