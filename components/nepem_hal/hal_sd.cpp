#include "hal_sd.h"
#include "hal_mock_defaults.h"
#ifndef SIMULATOR
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#endif
#include <string.h>

static const char* TAG = "HAL_SD";

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5

bool HalSd::available = false;

bool HalSd::init() {
#ifndef SIMULATOR
    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = false,
        .use_one_fat = false
    };
    sdmmc_card_t* card;
    const char mount_point[] = "/sdcard";
    
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    
    spi_bus_config_t bus_cfg = {};
    bus_cfg.mosi_io_num = PIN_NUM_MOSI;
    bus_cfg.miso_io_num = PIN_NUM_MISO;
    bus_cfg.sclk_io_num = PIN_NUM_CLK;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.max_transfer_sz = 4000;
    
    ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return false;
    }
    
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = (gpio_num_t)PIN_NUM_CS;
    slot_config.host_id = (spi_host_device_t)host.slot;
    
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SD card VFS FAT");
        return false;
    }
    
    ESP_LOGI(TAG, "SD Card mounted");
#else
    ESP_LOGI(TAG, "Simulated SD initialized (local file)");
#endif
    available = true;
    return true;
}

bool HalSd::is_available() {
    return available;
}

bool HalSd::append_file(const char* path, const char* data) {
    if (!available) return false;
    
#ifdef SIMULATOR
    const char* actual_path = "sim_log.csv";
#else
    const char* actual_path = path;
#endif

    FILE* f = fopen(actual_path, "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for appending");
        return false;
    }
    fprintf(f, "%s", data);
    fclose(f);
    return true;
}
