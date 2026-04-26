#include "hal_i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "HAL_I2C";

esp_err_t HalI2c::i2c_master_init() {
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0;
    
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) return err;
    
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void HalI2c::init() {
    esp_err_t err = i2c_master_init();
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "I2C Inicializado com sucesso");
    } else {
        ESP_LOGE(TAG, "Falha ao inicializar I2C");
    }
}

bool HalI2c::read_sht31(float &temp, float &hum) {
    uint8_t cmd[2] = {0x24, 0x00}; // Clock stretching disabled, high repeatability
    
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT31_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd_handle, cmd, 2, true);
    i2c_master_stop(cmd_handle);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);
    
    if (ret != ESP_OK) return false;

    vTaskDelay(20 / portTICK_PERIOD_MS); // Tempo de conversão (High repeatability max 15ms)

    uint8_t data[6];
    cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT31_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd_handle, data, 5, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd_handle, data + 5, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);

    if (ret != ESP_OK) return false;

    // TODO: Adicionar checagem de CRC nos bytes data[2] e data[5] se necessário para maior robustez

    uint16_t st = (data[0] << 8) | data[1];
    uint16_t srh = (data[3] << 8) | data[4];

    temp = -45.0f + (175.0f * ((float)st / 65535.0f));
    hum = 100.0f * ((float)srh / 65535.0f);

    return true;
}
