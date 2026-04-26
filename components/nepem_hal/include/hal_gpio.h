#pragma once

#ifndef SIMULATOR
#include "driver/gpio.h"
#else
typedef int gpio_num_t;
#endif

// Definições de Pinos
#define PIN_RELE_BOMBA (gpio_num_t)26
#define PIN_RELE_VENT (gpio_num_t)27
#define PIN_POWER_DETECT (gpio_num_t)32

class HalGpio {
public:
  static void init();
  static void set_pump(bool state);
  static void set_fan(bool state);
  static bool is_pump_on();
  static bool is_fan_on();
  static bool is_ac_power_present();

private:
  static bool pump_state;
  static bool fan_state;
};
