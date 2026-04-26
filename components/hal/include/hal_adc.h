#pragma once

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"

// Pinos capacitivos 34 = ADC1_CH6, 35 = ADC1_CH7
#define ADC_CH_CAP_1 ADC_CHANNEL_6
#define ADC_CH_CAP_2 ADC_CHANNEL_7

class HalAdc {
public:
  static void init();
  // Retorna a umidade média do solo em %, ou -1 em caso de falha crítica
  static float read_soil_moisture();

private:
  static adc_oneshot_unit_handle_t adc1_handle;
  static adc_cali_handle_t cali_handle;
  static float raw_to_percentage(int raw);
};
