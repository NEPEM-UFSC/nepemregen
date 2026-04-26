#include "utils.hpp"
#include "esp_system.h"
#include "esp_mac.h"
#include <sstream>
#include <iomanip>

namespace utils {
    std::string get_device_id() {
        uint8_t mac[6];
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
        std::stringstream ss;
        for(int i = 0; i < 6; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)mac[i];
        }
        return ss.str();
    }
}
