#pragma once
#include "hal_mock_defaults.h"

#ifndef SIMULATOR
#include "driver/i2c.h"
#else
#define I2C_NUM_0 0
#endif

#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0

#define SHT31_I2C_ADDR              0x44

class HalI2c {
public:
    static void init();
    static bool read_sht31(float &temp, float &hum);

private:
    static esp_err_t i2c_master_init();
};
