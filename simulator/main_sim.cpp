#include "hal_gpio.h"
#include "hal_adc.h"
#include "hal_i2c.h"
#include "hal_sd.h"
#include "ventilation_ctrl.h"
#include "irrigation_ctrl.h"
#include "telemetry.h"
#include "time_utils.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Starting NEPEM REGEN Simulator..." << std::endl;

    // Initialize simulation
    TimeUtils::init();
    HalGpio::init();
    HalAdc::init();
    HalI2c::init();
    HalSd::init();
    Telemetry::init();
    VentilationCtrl::init();
    IrrigationCtrl::init();

    float temp, hum, soil;
    bool business_hours = true;

    // Simulation loop
    for (int i = 0; i < 30; ++i) { // Run for 30 iterations
        // 1. Read simulated sensors
        soil = HalAdc::read_soil_moisture();
        HalI2c::read_sht31(temp, hum);

        // Inject critical value at iteration 10
        if (i >= 10 && i < 12) {
            soil = 40.0f; // Below SETPOINT_CRITICAL (50.0)
            std::cout << "[SIM] Injecting CRITICAL soil moisture: " << soil << "%" << std::endl;
        }

        std::cout << "\n--- Iteration " << i << " ---" << std::endl;
        std::cout << "Sensors -> Temp: " << temp << "C | Hum: " << hum << "% | Soil: " << soil << "%" << std::endl;

        // 2. Process Control Logic
        VentilationCtrl::process(temp, hum, soil, business_hours);
        IrrigationCtrl::process(soil, temp, hum);

        // 3. Status
        std::cout << "Status  -> Pump: " << (HalGpio::is_pump_on() ? "ON" : "OFF") 
                  << " | Fan: " << (HalGpio::is_fan_on() ? "ON" : "OFF") << std::endl;

        // Sleep for 1 simulated second
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "\nSimulation finished. Check 'sim_log.csv' for telemetry data." << std::endl;
    return 0;
}
