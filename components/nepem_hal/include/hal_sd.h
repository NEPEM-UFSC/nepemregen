#pragma once

#include "esp_err.h"

class HalSd {
public:
    static bool init();
    static bool append_file(const char* path, const char* data);
    static bool is_available();

private:
    static bool available;
};
