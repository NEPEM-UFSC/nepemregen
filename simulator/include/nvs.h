#pragma once
#include "hal_mock_defaults.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t nvs_open(const char* name, int mode, nvs_handle_t* handle);
esp_err_t nvs_get_i32(nvs_handle_t handle, const char* key, int32_t* out_value);
esp_err_t nvs_set_i32(nvs_handle_t handle, const char* key, int32_t value);
esp_err_t nvs_commit(nvs_handle_t handle);
void nvs_close(nvs_handle_t handle);

#ifdef __cplusplus
}
#endif
