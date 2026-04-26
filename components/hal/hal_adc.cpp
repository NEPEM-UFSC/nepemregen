#include "hal_adc.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include <math.h>

static const char *TAG = "HAL_ADC";

adc_oneshot_unit_handle_t HalAdc::adc1_handle = nullptr;
adc_cali_handle_t HalAdc::cali_handle = nullptr;

void HalAdc::init() {
  // 1. Initialize ADC Unit
  adc_oneshot_unit_init_config_t init_config1 = {
      .unit_id = ADC_UNIT_1,
      .clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
      .ulp_mode = ADC_ONESHOT_ulp_mode_DISABLE,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

  // 2. Configure Channels
  adc_oneshot_chan_config_t config = {
      .atten = ADC_ATTEN_DB_12, // ESP-IDF v6 equivalent of DB_11 is usually 12
                                // for some targets, or just use 12
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc1_handle, ADC_CH_CAP_1, &config));
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc1_handle, ADC_CH_CAP_2, &config));

  // 3. Calibration
  adc_cali_line_fitting_config_t cali_config = {
      .unit_id = ADC_UNIT_1,
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  // Note: If line fitting fails, we could try curve fitting or skip calibration
  esp_err_t ret =
      adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle);
  if (ret != ESP_OK) {
    ESP_LOGW(TAG, "Calibration failed, proceeding without it.");
    cali_handle = nullptr;
  }
}

float HalAdc::raw_to_percentage(int raw) {
  // Mapeamento: 1200 (na água) -> 100%, 4095 (no ar) -> 0%
  float percent = 100.0f - ((raw - 1200.0f) / (4095.0f - 1200.0f)) * 100.0f;
  if (percent < 0.0f)
    percent = 0.0f;
  if (percent > 100.0f)
    percent = 100.0f;
  return percent;
}

float HalAdc::read_soil_moisture() {
  int raw1 = 0, raw2 = 0;

  // Multisample para estabilidade
  for (int i = 0; i < 10; i++) {
    int temp_raw;
    adc_oneshot_read(adc1_handle, ADC_CH_CAP_1, &temp_raw);
    raw1 += temp_raw;
    adc_oneshot_read(adc1_handle, ADC_CH_CAP_2, &temp_raw);
    raw2 += temp_raw;
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
      ESP_LOGE(
          TAG,
          "Falha de Hardware: Diferenca entre sensores > 15%% (%.1f vs %.1f)",
          p1, p2);
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
