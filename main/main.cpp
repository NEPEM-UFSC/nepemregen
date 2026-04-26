#include "edge_analytics.hpp"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "irrigation_ctrl.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "telemetry.h"
#include "time_utils.h"
#include "ventilation_ctrl.h"
#include <time.h>

static const char *TAG = "MAIN";

// Variaveis globais de estado
float temp_ar = 0.0f;
float umid_ar = 0.0f;
float umid_solo = 0.0f;

// Filtros EMA (alpha = 0.1 para forte filtragem)
edge_analytics::EMAFilter ema_solo(0.1f);
edge_analytics::EMAFilter ema_temp(0.2f);
edge_analytics::EMAFilter ema_umid(0.2f);

// Base temp for GDD (e.g. 10C)
const float GDD_T_BASE = 10.0f;

// Tarefa de Aquisição Sensorial (A cada 5 segundos)
void task_sensors(void *pvParameters) {
  uint32_t last_day = TimeUtils::get_epoch() / 86400;
  bool last_power_state = HalGpio::is_ac_power_present();
  uint32_t power_lost_millis = 0;

  while (1) {
    float raw_solo = HalAdc::read_soil_moisture();
    float raw_temp, raw_umid;
    HalI2c::read_sht31(raw_temp, raw_umid);

    // Aplica o filtro passa-baixa (EMA)
    umid_solo = ema_solo.filter(raw_solo);
    temp_ar = ema_temp.filter(raw_temp);
    umid_ar = ema_umid.filter(raw_umid);

    // Atualiza rastreamento de temperatura para GDD
    edge_analytics::update_gdd(temp_ar, GDD_T_BASE);

    // Verifica fechamento diário do GDD
    uint32_t current_day = TimeUtils::get_epoch() / 86400;
    if (current_day != last_day && current_day > 0) {
      edge_analytics::commit_daily_gdd(GDD_T_BASE);
      last_day = current_day;
      ESP_LOGI(TAG, "GDD diario computado e salvo no RTC!");
    }

    // Monitoramento de Falha de Energia
    bool current_power_state = HalGpio::is_ac_power_present();
    if (current_power_state != last_power_state) {
      if (current_power_state == false) {
        power_lost_millis = esp_log_timestamp();
        ESP_LOGE(TAG,
                 "ALERTA: Queda de energia da rede. Operando via Bateria/UPS.");
        Telemetry::log_event("Hardware", "FALHA_ENERGIA", temp_ar, umid_ar,
                             umid_solo, false);
        power_lost_millis = TimeUtils::millis();

        // Atualiza NVS com o contador de quedas
        nvs_handle_t my_handle;
        if (nvs_open("regen_stats", NVS_READWRITE, &my_handle) == ESP_OK) {
          int32_t quedas = 0;
          nvs_get_i32(my_handle, "counter_quedas", &quedas);
          quedas++;
          nvs_set_i32(my_handle, "counter_quedas", quedas);
          nvs_commit(my_handle);
          nvs_close(my_handle);
          ESP_LOGI(TAG, "Queda registrada no NVS. Total historico: %ld",
                   (long)quedas);
        }
      } else {
        uint32_t duration_sec =
            (TimeUtils::millis() - power_lost_millis) / 1000;
        ESP_LOGI(
            TAG,
            "Energia da rede restaurada. Autonomia utilizada: %lu segundos.",
            (unsigned long)duration_sec);
        Telemetry::log_event("Hardware", "ENERGIA_RESTAURADA", temp_ar, umid_ar,
                             umid_solo, false);
      }
      last_power_state = current_power_state;
    }

    ESP_LOGI(TAG, "Sensores - Ar: %.2fC | %.2f%% | Solo: %.1f%%", temp_ar,
             umid_ar, umid_solo);
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

// Tarefa de Controle (A cada 1 segundo)
void task_control(void *pvParameters) {
  while (1) {
    // Horario comercial mock (sempre true para simplificar ou dependeria do
    // sntp real)
    bool is_business_hours = true;

    VentilationCtrl::process(temp_ar, umid_ar, umid_solo, is_business_hours);

    // IrrigationCtrl é bloqueante durante T_PULSE, mas roda numa task separada
    // para não bloquear a ventilação! Correção: Para não bloquear a task de
    // ventilação, criamos uma logica não bloqueante ou separamos tasks. Como o
    // design em irrigation_ctrl possui delay, vamos chama-la em uma task
    // própria.
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void task_irrigation(void *pvParameters) {
  while (1) {
    IrrigationCtrl::process(umid_solo, temp_ar, umid_ar);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// Tarefa de Log Temporizado (A cada 30 minutos)
void task_telemetry_timer(void *pvParameters) {
  const TickType_t xDelay = pdMS_TO_TICKS(1800000); // 30 minutos
  while (1) {
    vTaskDelay(xDelay);
    Telemetry::log_event("Tempo_30m", "NENHUM", temp_ar, umid_ar, umid_solo,
                         HalGpio::is_pump_on() ? false : false);
  }
}

extern "C" void app_main() {
  // Inicializa NVS (necessario para armazenar contadores)
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  ESP_LOGI(TAG, "Iniciando NEPEM REGEN Edge Appliance...");

  // Checa causa do reset para detectar Brownout (Surto/Instabilidade)
  esp_reset_reason_t reason = esp_reset_reason();
  if (reason == ESP_RST_BROWNOUT) {
    ESP_LOGE(TAG,
             "CRITICAL: System rebooted due to BROWNOUT (Power Surge/Dip).");
    // Sera logado via telemetria quando a task de controle iniciar
  } else {
    ESP_LOGI(TAG, "Reset reason: %d", reason);
  }

  // Leitura do NVS para reportar histórico de quedas no boot
  nvs_handle_t my_handle;
  if (nvs_open("regen_stats", NVS_READONLY, &my_handle) == ESP_OK) {
    int32_t quedas = 0;
    nvs_get_i32(my_handle, "counter_quedas", &quedas);
    nvs_close(my_handle);
    ESP_LOGI(TAG, "Total de quedas de energia registradas no NVS: %ld",
             (long)quedas);
  }

  TimeUtils::init();
  HalGpio::init();
  HalAdc::init();
  HalI2c::init();
  Telemetry::init();
  IrrigationCtrl::init();
  VentilationCtrl::init();

  // Cria as tasks
  xTaskCreate(task_sensors, "Task_Sensors", 4096, NULL, 5, NULL);
  xTaskCreate(task_control, "Task_Control_Vent", 4096, NULL, 4, NULL);
  xTaskCreate(task_irrigation, "Task_Irrigation", 4096, NULL, 3, NULL);
  xTaskCreate(task_telemetry_timer, "Task_Telemetry", 4096, NULL, 2, NULL);
}