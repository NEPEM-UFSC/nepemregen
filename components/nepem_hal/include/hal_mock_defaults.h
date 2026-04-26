#pragma once

#ifdef SIMULATOR
#include <stdio.h>
#include <cstdint>
#include <cstring>
#include <string>

#ifndef ESP_LOGI
#define ESP_LOGI(tag, fmt, ...) printf("INFO [%s]: " fmt "\n", tag, ##__VA_ARGS__)
#endif

#ifndef RTC_DATA_ATTR
#define RTC_DATA_ATTR
#endif

#ifndef ESP_LOGW
#define ESP_LOGW(tag, fmt, ...) printf("WARN [%s]: " fmt "\n", tag, ##__VA_ARGS__)
#endif

#ifndef ESP_LOGE
#define ESP_LOGE(tag, fmt, ...) printf("ERROR [%s]: " fmt "\n", tag, ##__VA_ARGS__)
#endif

#ifndef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x) (void)(x)
#endif

#ifndef ESP_OK
#define ESP_OK 0
#endif

typedef int esp_err_t;
typedef int adc_channel_t;

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS(x) (x)
#endif

// I2C Mocks
#define I2C_MODE_MASTER 0
#define I2C_MASTER_WRITE 0
#define I2C_MASTER_READ 1
#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

// GPIO Mocks
#define GPIO_PULLUP_ENABLE 1
#define GPIO_PULLDOWN_DISABLE 0
#define GPIO_PULLUP_DISABLE 0
#define GPIO_MODE_OUTPUT 0
#define GPIO_MODE_INPUT 1
#define GPIO_INTR_DISABLE 0

#ifndef portTICK_PERIOD_MS
#define portTICK_PERIOD_MS 1
#endif

inline void vTaskDelay(int ticks) { (void)ticks; }


#endif
