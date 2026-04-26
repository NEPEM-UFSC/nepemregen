cmake_minimum_required(VERSION 3.16)
project(nepem_simulator)

set(CMAKE_CXX_STANDARD 17)

# Define SIMULATOR macro
add_definitions(-DSIMULATOR=1)

# Include paths
include_directories(
    ../components/nepem_hal/include
    ../components/app_core/include
    ../components/utils/include
)

# Source files
set(SOURCES
    main_sim.cpp
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

# Mocks for edge_analytics and telemetry if needed, or include them if they are clean
# Let's check them first.
add_executable(nepem_sim ${SOURCES})
