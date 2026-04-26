#pragma once

#include <stdint.h>

class TimeUtils {
public:
    static void init();
    // Retorna uptime em milisegundos
    static uint32_t millis();
    // Retorna epoch atual (mesmo que ntp não sincronizado)
    static uint32_t get_epoch();
};
