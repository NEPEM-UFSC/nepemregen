#include "time_utils.h"
#include "hal_mock_defaults.h"
#ifndef SIMULATOR
#include "esp_timer.h"
#endif
#include <sys/time.h>
#include <time.h>
#ifdef SIMULATOR
#include <chrono>
#endif

void TimeUtils::init() {
  // Aqui poderiamos inicializar SNTP se fosse usado Wi-Fi
}

uint32_t TimeUtils::millis() {
#ifndef SIMULATOR
  return (uint32_t)(esp_timer_get_time() / 1000ULL);
#else
  static auto start = std::chrono::steady_clock::now();
  auto now = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
#endif
}

uint32_t TimeUtils::get_epoch() {
  time_t now;
  time(&now);
  return (uint32_t)now;
}
