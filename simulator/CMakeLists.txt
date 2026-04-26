cmake_minimum_required(VERSION 3.16)
project(nepem_simulator)

set(CMAKE_CXX_STANDARD 17)

# Define SIMULATOR macro
add_definitions(-DSIMULATOR=1)

# ESP-IDF Unity path
set(UNITY_PATH "C:/esp/v6.0/esp-idf/components/unity/unity/src")

# Include paths
include_directories(
    ../components/nepem_hal/include
    ../components/app_core/include
    ../components/utils/include
    ./include
    ${UNITY_PATH}
)

# Source files
set(SOURCES
    nvs_mock.cpp
    ../components/nepem_hal/hal_gpio.cpp
    ../components/nepem_hal/hal_adc.cpp
    ../components/nepem_hal/hal_i2c.cpp
    ../components/nepem_hal/hal_sd.cpp
    ../components/app_core/ventilation_ctrl.cpp
    ../components/app_core/irrigation_ctrl.cpp
    ../components/app_core/telemetry.cpp
    ../components/utils/time_utils.cpp
    ../components/utils/edge_analytics.cpp
)

# Main simulator executable
add_executable(nepem_sim main_sim.cpp ${SOURCES})

# Unit tests executable
add_executable(nepem_tests 
    run_tests.cpp 
    ../test/main/test_analytics.cpp 
    ../test/main/test_irrigation.cpp
    ../test/main/test_integration.cpp
    ${SOURCES}
    ${UNITY_PATH}/unity.c
)
