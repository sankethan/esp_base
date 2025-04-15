

#include "storage.h"

Storage::Storage(){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open
    printf("Opening Non-Volatile Storage (NVS) handle... \n");

    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        ESP_ERROR_CHECK(err);
    }
};

esp_err_t Storage::get_u8(const char *key, uint8_t *val)
{
    return nvs_get_u8(my_handle, key, val);
}
esp_err_t Storage::set_u8(const char *key, uint8_t val)
{
    esp_err_t err = nvs_set_u8(my_handle, key, val);
    ESP_ERROR_CHECK(err);
    err = nvs_commit(my_handle);
    return err;
}
esp_err_t Storage::set_int(const char *key, int32_t val)
{
    esp_err_t err = nvs_set_i32(my_handle, key, val);
    ESP_ERROR_CHECK(err);
    err = nvs_commit(my_handle);

    return err;
}
esp_err_t Storage::get_int(const char *key, int32_t *val)
{
    return nvs_get_i32(my_handle, key, val);
}
esp_err_t Storage::set_str(const char *key, const char *val)
{
    esp_err_t err = nvs_set_str(my_handle, key, val);
    ESP_ERROR_CHECK(err);
    err = nvs_commit(my_handle);
    return err;
}
esp_err_t Storage::get_str(const char *key, char *val)
{
    size_t siz = 17;
    return nvs_get_str(my_handle, key, val, &siz);
}
Storage::~Storage()
{
    nvs_close(my_handle);
}