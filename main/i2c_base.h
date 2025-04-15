#ifndef I2C_BASE_H
#define I2C_BASE_H

#include "driver/i2c_master.h"


class i2c_base
{
private:
    uint8_t dev_address;
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;

public:
    i2c_base(uint8_t addr);
    void populate_arr(uint8_t *_arr, int n, uint8_t reg_addr, uint8_t *data);
    esp_err_t i2c_register_read(uint8_t reg_addr, uint8_t *data, size_t len);
    esp_err_t i2c_register_write_n_bytes(uint8_t reg_addr, uint8_t *data, int n);
    esp_err_t i2c_register_write_word(uint8_t reg_addr, uint16_t data);
};

void print_n_bytes(const char *_c, uint8_t *arr, int n);

#endif