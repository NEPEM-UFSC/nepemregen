#include "hal_adc.h"
#include "esp_log.h"
#include <math.h>

static const char* TAG = "HAL_ADC";

esp_adc_cal_characteristics_t* HalAdc::adc_chars = nullptr;

void HalAdc::init() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CH_CAP_1, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC_CH_CAP_2, ADC_ATTEN_DB_11);

    adc_chars = (esp_adc_cal_characteristics_t*) calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, adc_chars);
}

// Mapeamento: 1200 (na água) -> 100%, 4095 (no ar) -> 0%
float HalAdc::raw_to_percentage(int raw) {
    float percent = 100.0f - ((raw - 1200.0f) / (4095.0f - 1200.0f)) * 100.0f;
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 100.0f) percent = 100.0f;
    return percent;
}

float HalAdc::read_soil_moisture() {
    int raw1 = 0, raw2 = 0;
    
    // Multisample para estabilidade
    for(int i=0; i<10; i++) {
        raw1 += adc1_get_raw(ADC_CH_CAP_1);
        raw2 += adc1_get_raw(ADC_CH_CAP_2);
    }
    raw1 /= 10;
    raw2 /= 10;

    float p1 = raw_to_percentage(raw1);
    float p2 = raw_to_percentage(raw2);

    bool valid1 = (raw1 > 1000 && raw1 < 4096); // sanity check raw
    bool valid2 = (raw2 > 1000 && raw2 < 4096);

    if (valid1 && valid2) {
        float diff = fabs(p1 - p2);
        if (diff > 15.0f) {
            ESP_LOGE(TAG, "Falha de Hardware: Diferenca entre sensores > 15%% (%.1f vs %.1f)", p1, p2);
            // Poderíamos retornar -1, mas o readme pede log de erro.
            // Para não quebrar a lógica hídrica, podemos usar a média e o log avisa a telemetria.
        }
        return (p1 + p2) / 2.0f;
    } else if (valid1) {
        ESP_LOGW(TAG, "Sensor 2 inativo ou fora de escala. Usando S1.");
        return p1;
    } else if (valid2) {
        ESP_LOGW(TAG, "Sensor 1 inativo ou fora de escala. Usando S2.");
        return p2;
    }

    ESP_LOGE(TAG, "Ambos sensores falharam");
    return -1.0f;
}
