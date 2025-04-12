#include <inttypes.h>
#include <stdio.h>
#include "i2c_base.h"
#include "driver/i2c_master.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

i2c_base::i2c_base(uint8_t addr)
{
    i2c_master_bus_config_t i2c_mst_config = {
        .i2c_port = 1,
        .sda_io_num = GPIO_NUM_4,
        .scl_io_num = GPIO_NUM_2,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags= {1,0}};
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
}

esp_err_t i2c_base::i2c_register_write_n_bytes(uint8_t reg_addr, uint8_t *data, int n)
{
    if (n <= 0 || n > 8)
        return -1;
    n++;
    uint8_t *_arr = new uint8_t[n];
    populate_arr(_arr, n, reg_addr, data);

    int i2c_res = i2c_master_transmit(dev_handle, _arr, sizeof(_arr), 1000);
    delete[] _arr;
    return i2c_res;
}

esp_err_t i2c_base::i2c_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, 1000);
}

void i2c_base::populate_arr(uint8_t *_arr, int n, uint8_t reg_addr, uint8_t *data)
{
    _arr[0] = reg_addr;
    for (int i = 1; i < n; i++)
        _arr[i] = data[i - 1];
}


void LDC1612::print_n_bytes(const char* _c,uint8_t *arr, int n){
    int i = 0;
    printf("%s",_c);
    while (i < n)
    {

        printf("0x%X ", arr[i]);
        i++;
    }
    printf("\n");
}

void LDC1612::check_who_am_i()
{
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DEVICE_ID, buffer, reg_size));

    print_n_bytes("WHO_AM_I = ", buffer, reg_size);

    if ((buffer[0] << 8 | buffer[1]) != 0x3055)
        log.error_log("Device Address are not the same!");
    else
        log.success_log("Device Address matched!");
}

void LDC1612::get_l2(bool print)
{

    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA0_MSB, buffer, 2));
    L2[0] = ((uint16_t)buffer[0] << 8) | buffer[1];

    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA1_MSB, buffer, 2));
    L2[2] = ((uint16_t)buffer[0] << 8) | buffer[1];

    if (print)
    {
        printf("AX= %x\n", L2[0]);
        printf("AY= %x\n", L2[1]);
    }
}


void LDC1612::reset(){
    uint8_t _data[2] = {0x80,0x00};
    ESP_ERROR_CHECK(i2c_driver.i2c_register_write_n_bytes(LDC_RESET_DEV,_data,2));
}




LDC1612::LDC1612(uint8_t addr, int reg_size): i2c_driver(addr), reg_size(reg_size), log(MEDIUM){
    L2[0]=0;
    L2[1]=0;
    reset();
    check_who_am_i();
    uint8_t _data[2] = {0x00,0x01};
    ESP_ERROR_CHECK(i2c_driver.i2c_register_write_n_bytes(LDC_CONFIG, _data,2));
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_CONFIG, buffer, 2));
    printf("CONFIG = %X %X\n", buffer[0], buffer[1]);
}
