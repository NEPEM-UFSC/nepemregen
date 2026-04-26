#include "hal_gpio.h"

bool HalGpio::pump_state = false;
bool HalGpio::fan_state = false;
#ifdef SIMULATOR
bool HalGpio::was_pump_ever_on = false;
void HalGpio::reset_history() { was_pump_ever_on = false; }
#endif

void HalGpio::init() {
#ifndef SIMULATOR
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = (1ULL << PIN_RELE_BOMBA) | (1ULL << PIN_RELE_VENT);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);

  gpio_config_t in_conf = {};
  in_conf.intr_type = GPIO_INTR_DISABLE;
  in_conf.mode = GPIO_MODE_INPUT;
  in_conf.pin_bit_mask = (1ULL << PIN_POWER_DETECT);
  in_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  in_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&in_conf);
#endif

  set_pump(false);
  set_fan(false);
}

void HalGpio::set_pump(bool state) {
  pump_state = state;
#ifdef SIMULATOR
  if (state)
    was_pump_ever_on = true;
#endif
#ifndef SIMULATOR
  gpio_set_level(PIN_RELE_BOMBA, state ? 1 : 0);
#endif
}

void HalGpio::set_fan(bool state) {
  fan_state = state;
#ifndef SIMULATOR
  gpio_set_level(PIN_RELE_VENT, state ? 1 : 0);
#endif
}

bool HalGpio::is_pump_on() { return pump_state; }

bool HalGpio::is_fan_on() { return fan_state; }

bool HalGpio::is_ac_power_present() {
#ifndef SIMULATOR
  return gpio_get_level(PIN_POWER_DETECT) == 1;
#else
  return true; // Assume power is present in simulator
#endif
}
