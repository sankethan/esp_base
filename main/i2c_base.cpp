#include "i2c_base.h"

i2c_base::i2c_base(uint8_t addr)
{
    i2c_master_bus_config_t i2c_mst_config = {
        .i2c_port = 1,
        .sda_io_num = GPIO_NUM_10,
        .scl_io_num = GPIO_NUM_9,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {1, 0}};
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

    print_n_bytes("WRITE: ", _arr, n);
    int i2c_res = i2c_master_transmit(dev_handle, _arr, sizeof(_arr), 1000);
    delete[] _arr;
    return i2c_res;
}

esp_err_t i2c_base::i2c_register_write_word(uint8_t reg_addr, uint16_t data)
{
    uint8_t write_buf[3] = {reg_addr, uint8_t((data & 0xFF00) >> 8), uint8_t(data & 0x00FF)};
    print_n_bytes("WRITE: ", write_buf, 3);
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), 1000);
}

esp_err_t i2c_base::i2c_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    int ret = i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, 1000);
    // print_n_bytes("READ: ", data, 2);
    return ret;
}

void i2c_base::populate_arr(uint8_t *_arr, int n, uint8_t reg_addr, uint8_t *data)
{
    _arr[0] = reg_addr;
    for (int i = 1; i < n; i++)
        _arr[i] = data[i - 1];
}

void print_n_bytes(const char *_c, uint8_t *arr, int n)
{
    int i = 0;
    printf("%s", _c);
    while (i < n)
    {

        printf("0x%X ", arr[i]);
        i++;
    }
    printf("\n");
}