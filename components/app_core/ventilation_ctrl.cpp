#include "ventilation_ctrl.h"
#include "edge_analytics.hpp"
#include "hal_gpio.h"
#include "telemetry.h"
#include "time_utils.h"
const uint32_t VentilationCtrl::WINDOW_30M_MS = 1800000;
const uint32_t VentilationCtrl::TARGET_5M_MS = 300000;
const float VentilationCtrl::TEMP_MAX = 32.0f;
const float VentilationCtrl::HUM_MAX = 80.0f;

uint32_t VentilationCtrl::window_start_time = 0;
uint32_t VentilationCtrl::accumulated_fan_time = 0;
uint32_t VentilationCtrl::fan_turn_on_time = 0;

void VentilationCtrl::init() {
  window_start_time = TimeUtils::millis();
  accumulated_fan_time = 0;
  fan_turn_on_time = 0;
}

void VentilationCtrl::process(float current_temp, float current_hum,
                              float current_soil_moisture,
                              bool is_business_hours) {
  uint32_t now = TimeUtils::millis();
  bool emergency = (current_temp > TEMP_MAX && current_hum > HUM_MAX);

  float vpd_atual = edge_analytics::calculate_vpd(current_temp, current_hum);

  if (vpd_atual < 0.4f) {
    emergency = true; // Risco critico de condensacao/fungos
  } else if (vpd_atual > 1.6f) {
    // Estresse hidrico extremo
    // Log for python
    Telemetry::log_event("Alerta_VPD_Alto", "ESTRESSE_HIDRICO", current_temp,
                         current_hum, current_soil_moisture, false);
  }

  // Reseta janela a cada 30 min
  if (now - window_start_time >= WINDOW_30M_MS) {
    window_start_time = now;
    accumulated_fan_time = 0;
  }

  // Calcula tempo se ventilador estivesse ligado pra não acumular erro
  uint32_t temp_acc = accumulated_fan_time;
  if (HalGpio::is_fan_on()) {
    temp_acc += (now - fan_turn_on_time);
  }

  bool deficit_ventilacao = false;
  // Se faltar pouco tempo pra acabar a janela e ainda não bateu a meta
  if ((now - window_start_time) > (WINDOW_30M_MS - TARGET_5M_MS)) {
    if (temp_acc < TARGET_5M_MS) {
      deficit_ventilacao = true;
    }
  }

  if (emergency || (deficit_ventilacao && is_business_hours)) {
    if (!HalGpio::is_fan_on()) {
      HalGpio::set_fan(true);
      fan_turn_on_time = now;
      const char *trigger = emergency ? "Emergencia_Ar" : "Regra_Manutencao";
      Telemetry::log_event(trigger, "VENT_LIGOU", current_temp, current_hum,
                           current_soil_moisture,
                           false); // Lag aqui não se aplica
    }
  } else {
    if (HalGpio::is_fan_on() && !emergency) {
      // Pode desligar
      HalGpio::set_fan(false);
      accumulated_fan_time += (now - fan_turn_on_time);
      Telemetry::log_event("Regra_Manutencao", "VENT_DESLIGOU", current_temp,
                           current_hum, current_soil_moisture, false);
    }
  }
}
