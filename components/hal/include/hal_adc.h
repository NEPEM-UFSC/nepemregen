#pragma once

#include "driver/adc.h"
#include "esp_adc_cal.h"

// Pinos capacitivos 34 = ADC1_CH6, 35 = ADC1_CH7
#define ADC_CH_CAP_1 ADC1_CHANNEL_6
#define ADC_CH_CAP_2 ADC1_CHANNEL_7

class HalAdc {
public:
    static void init();
    // Retorna a umidade média do solo em %, ou -1 em caso de falha crítica
    static float read_soil_moisture();

private:
    static esp_adc_cal_characteristics_t *adc_chars;
    static float raw_to_percentage(int raw);
};
