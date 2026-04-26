#include "irrigation_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal_gpio.h"
#include "telemetry.h"
#include "time_utils.h"

const float IrrigationCtrl::SETPOINT_CRITICAL = 50.0f;
const uint32_t IrrigationCtrl::T_PULSE_MS = 10000; // 10s
const uint32_t IrrigationCtrl::T_LAG_MS = 600000;  // 10m

uint32_t IrrigationCtrl::last_pulse_time = 0;
bool IrrigationCtrl::in_cooldown = false;

void IrrigationCtrl::init() {
  last_pulse_time = 0;
  in_cooldown = false;
}

void IrrigationCtrl::process(float current_soil_moisture, float current_temp,
                             float current_hum) {
  if (current_soil_moisture < 0.0f)
    return; // Erro de sensor, não irriga

  uint32_t now = TimeUtils::millis();

  // Check se saiu do cooldown
  if (in_cooldown) {
    if ((now - last_pulse_time) >= T_LAG_MS) {
      in_cooldown = false;
    } else {
      return; // Bloqueado pelo interlock t_lag
    }
  }

  if (current_soil_moisture < SETPOINT_CRITICAL) {
    // Ligar Bomba
    HalGpio::set_pump(true);
    Telemetry::log_event("Sensor_Solo", "BOMBA_LIGOU", current_temp,
                         current_hum, current_soil_moisture, false);

    // Mantem ligada por t_p (bloqueante para garantir o volume,
    // ou poderia ser não bloqueante, mas para bomba o vTaskDelay de 10s é
    // aceitável na task de controle hídrico)
    vTaskDelay(T_PULSE_MS / portTICK_PERIOD_MS);

    // Desligar Bomba
    HalGpio::set_pump(false);
    last_pulse_time =
        TimeUtils::millis(); // Atualiza o tempo pro inicio do cooldown
    in_cooldown = true;
    Telemetry::log_event("Timer_Pulso", "BOMBA_DESLIGOU", current_temp,
                         current_hum, current_soil_moisture, true);
  }
}
