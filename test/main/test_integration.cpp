#include "unity.h"
#include "nvs_flash.h"
#include "nvs.h"

void test_nvs_persistence_after_reset(void) {
    nvs_handle_t handle;
    int32_t pheno_state_save = 2; // Floração
    int32_t pheno_state_load = 0;
    
    // 1. Setup NVS
    nvs_flash_erase();
    nvs_flash_init();
    
    // 2. Save value
    TEST_ASSERT_EQUAL(0, nvs_open("storage", NVS_READWRITE, &handle));
    TEST_ASSERT_EQUAL(0, nvs_set_i32(handle, "pheno_state", pheno_state_save));
    TEST_ASSERT_EQUAL(0, nvs_commit(handle));
    nvs_close(handle);
    
    // 3. "Reset" - simulate by re-initializing the flash mock
    nvs_flash_init(); 
    
    // 4. Load value
    TEST_ASSERT_EQUAL(0, nvs_open("storage", NVS_READONLY, &handle));
    TEST_ASSERT_EQUAL(0, nvs_get_i32(handle, "pheno_state", &pheno_state_load));
    nvs_close(handle);
    
    // 5. Verify
    TEST_ASSERT_EQUAL_INT32(pheno_state_save, pheno_state_load);
}
