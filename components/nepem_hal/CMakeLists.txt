idf_component_register(SRCS "hal_gpio.cpp" "hal_adc.cpp" "hal_i2c.cpp" "hal_sd.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES driver fatfs vfs sdmmc esp_adc)
