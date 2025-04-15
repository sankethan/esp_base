#ifndef STORAGE_H
#define STORAGE_H

#include "nvs_flash.h"
#include "nvs.h"
class Storage
{
private:
    nvs_handle_t my_handle;
public:
    Storage();
    esp_err_t get_u8(const char *key, uint8_t *val);
    esp_err_t set_u8(const char *key, uint8_t val);
    esp_err_t set_int(const char *key, int32_t val);
    esp_err_t get_int(const char *key, int32_t *val);
    esp_err_t set_str(const char *key, const char *val);
    esp_err_t get_str(const char *key, char *val);
    ~Storage();
};
#endif