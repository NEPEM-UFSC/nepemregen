idf_component_register(SRCS "app_core.cpp" "telemetry.cpp" "irrigation_ctrl.cpp" "ventilation_ctrl.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES hal utils)
