#include "nvs.h"
#include "nvs_flash.h"
#include <map>
#include <string>
#include <fstream>
#include <iostream>

static std::map<std::string, int32_t> nvs_data;
const char* NVS_FILE = "sim_nvs.bin";

static void load_nvs() {
    std::ifstream f(NVS_FILE, std::ios::binary);
    if (!f) return;
    size_t count;
    f.read((char*)&count, sizeof(count));
    for (size_t i = 0; i < count; ++i) {
        size_t key_len;
        f.read((char*)&key_len, sizeof(key_len));
        std::string key(key_len, ' ');
        f.read(&key[0], key_len);
        int32_t val;
        f.read((char*)&val, sizeof(val));
        nvs_data[key] = val;
    }
}

static void save_nvs() {
    std::ofstream f(NVS_FILE, std::ios::binary);
    size_t count = nvs_data.size();
    f.write((char*)&count, sizeof(count));
    for (auto const& [key, val] : nvs_data) {
        size_t key_len = key.length();
        f.write((char*)&key_len, sizeof(key_len));
        f.write(key.data(), key_len);
        f.write((char*)&val, sizeof(val));
    }
}

extern "C" {

esp_err_t nvs_flash_init() {
    load_nvs();
    return 0; // ESP_OK
}

esp_err_t nvs_flash_erase() {
    nvs_data.clear();
    remove(NVS_FILE);
    return 0;
}

esp_err_t nvs_open(const char* name, int mode, nvs_handle_t* handle) {
    *handle = 1; // Single handle for all
    return 0;
}

esp_err_t nvs_get_i32(nvs_handle_t handle, const char* key, int32_t* out_value) {
    if (nvs_data.find(key) == nvs_data.end()) {
        return ESP_ERR_NVS_NOT_FOUND;
    }
    *out_value = nvs_data[key];
    return 0;
}

esp_err_t nvs_set_i32(nvs_handle_t handle, const char* key, int32_t value) {
    nvs_data[key] = value;
    return 0;
}

esp_err_t nvs_commit(nvs_handle_t handle) {
    save_nvs();
    return 0;
}

void nvs_close(nvs_handle_t handle) {
    // Nothing to do
}

}
