#include "telemetry.h"
#include "edge_analytics.hpp"
#include "hal_mock_defaults.h"
#ifndef SIMULATOR
#include "esp_log.h"
#endif
#include "hal_sd.h"
#include "time_utils.h"
#include <stdio.h>
static const char *TAG = "TELEMETRY";
bool Telemetry::http_available = false; // Stub para futura implementação

void Telemetry::init() {
  // Tenta inicializar SD. Se falhar, dependemos apenas de HTTP (no futuro)
  if (HalSd::init()) {
    // Checa se o arquivo tem cabeçalho, se não, cria.
    HalSd::append_file("/sdcard/regen_log.csv",
                       "Timestamp,Trigger_Origem,Evento_Atuador,Temp_Ar,Umid_"
                       "Ar,Umid_Solo_Media,Lag_Ativo,VPD,Td,GDD\n");
  } else {
    ESP_LOGW(TAG, "SD Card não disponível. Logs serão apenas enviados via HTTP "
                  "se configurado no futuro.");
  }
}

void Telemetry::write_to_csv(const std::string &line) {
  if (HalSd::is_available()) {
    HalSd::append_file("/sdcard/regen_log.csv", line.c_str());
    ESP_LOGI(TAG, "Log salvo no SD: %s", line.c_str());
  }
  if (http_available) {
    send_http(line);
  }
}

void Telemetry::log_event(const char *trigger, const char *action, float temp,
                          float hum_ar, float hum_solo, bool lag_ativo) {
  uint32_t ts = TimeUtils::get_epoch();
  float vpd = edge_analytics::calculate_vpd(temp, hum_ar);
  float td = edge_analytics::calculate_dew_point(temp, hum_ar);
  float gdd = edge_analytics::get_accumulated_gdd();

  char buffer[300];
  snprintf(buffer, sizeof(buffer),
           "%u,%s,%s,%.2f,%.2f,%.2f,%d,%.3f,%.2f,%.2f\n", (unsigned int)ts,
           trigger, action, temp, hum_ar, hum_solo, lag_ativo ? 1 : 0, vpd, td,
           gdd);

  write_to_csv(std::string(buffer));
}

void Telemetry::send_http(const std::string &data) {
  // TODO: Implementar HTTP Client no futuro
  ESP_LOGI(TAG, "Stub HTTP: %s", data.c_str());
}
