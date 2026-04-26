#include "time_utils.h"
#include "esp_timer.h"
#include <sys/time.h>
#include <time.h>

void TimeUtils::init() {
  // Aqui poderiamos inicializar SNTP se fosse usado Wi-Fi
}

uint32_t TimeUtils::millis() {
  return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

uint32_t TimeUtils::get_epoch() {
  time_t now;
  time(&now);
  return (uint32_t)now;
}
