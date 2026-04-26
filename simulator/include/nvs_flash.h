#pragma once
#include "hal_mock_defaults.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t nvs_flash_init();
esp_err_t nvs_flash_erase();

#ifdef __cplusplus
}
#endif
