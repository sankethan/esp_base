/*
    Seeed_LDC1612.h
    Driver for LDC1612

    Website    : 
    Author     : Sankethan
    Create Time: April 2025
    Change Log :
*/

#include "LDC1612.h"
#include "math.h"

// LDC1612 class
LDC1612::LDC1612(uint8_t addr) : i2c_driver(addr)
{
    L2[0] = 0;
    L2[1] = 0;
    // reset();
    check_who_am_i();
    // uint16_t _data = 0x1001;

    single_channel_config(0);
    
    // i2c_driver.i2c_register_write_word(LDC_CONFIG, 0x2c01);
    // ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_CONFIG, buffer, 2));
    // printf("CONFIG = %X %X\n", buffer[0], buffer[1]);

    // set_reg(LDC_DRIVE_CURRENT0, 0x0000);
    // get_reg(LDC_DRIVE_CURRENT0, true, "LDC DRIVE CURR0: ");

    // set_reg(0x08, 0xFFFF);

    // uint16_t _data = 0x0c01;
    // i2c_driver.i2c_register_write_word(LDC_CONFIG, _data);
    // ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_CONFIG, buffer, 2));
    // printf("CONFIG = %X %X\n", buffer[0], buffer[1]);
}


void LDC1612::get_l2(bool print)
{

    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA0_MSB, buffer, 2));
    uint16_t msb_temp = ((uint16_t)buffer[0] << 8) | buffer[1];
    msb_temp &= 0x0fff;
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA0_LSB, buffer, 2));
    uint16_t lsb_temp = ((uint16_t)buffer[0] << 8) | buffer[1];
    L2[0] = ((uint32_t)msb_temp << 16) | lsb_temp;

    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA1_MSB, buffer, 2));
    msb_temp = ((uint16_t)buffer[0] << 8) | buffer[1];
    msb_temp &= 0x0fff;
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA1_LSB, buffer, 2));
    lsb_temp = ((uint16_t)buffer[0] << 8) | buffer[1];
    L2[1] = ((uint32_t)msb_temp << 16) | lsb_temp;

    if (print)
    {
        printf("AX= %lx\n", L2[0]);
        printf("AY= %lx\n", L2[1]);
    }
}

void LDC1612::get_l1(bool print)
{

    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA0_MSB, buffer, 2));
    uint16_t msb_temp = ((uint16_t)buffer[0] << 8) | buffer[1];
    // msb_temp&=0x0fff;
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA0_LSB, buffer, 2));
    uint16_t lsb_temp = ((uint16_t)buffer[0] << 8) | buffer[1];
    L2[0] = ((uint32_t)msb_temp << 16) | lsb_temp;

    if (print)
    {
        printf("AX= %ld(%lx)\n", L2[0], L2[0]);
        // printf("AY= %lx\n", L2[1]);
    }
}

int LDC1612::get_channel_result(uint8_t channel, bool print)
{
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA0_MSB+(channel*2), buffer, 2));
    uint16_t msb_temp = ((uint16_t)buffer[0] << 8) | buffer[1];
    // msb_temp&=0x0fff;
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DATA0_LSB+(channel*2), buffer, 2));
    uint16_t lsb_temp = ((uint16_t)buffer[0] << 8) | buffer[1];
    L2[channel] = ((uint32_t)msb_temp << 16) | lsb_temp;
    uint32_t result = L2[channel];

    // uint32_t raw_value = 0;
    // uint16_t value = 0;
    // i2c_driver.i2c_register_read(LDC_DATA0_MSB + (channel * 2), buffer,2);
    // raw_value |= (uint32_t)value << 16;
    // i2c_driver.i2c_register_read(LDC_DATA0_LSB + (channel * 2), buffer,2);
    // raw_value |= (uint32_t)value;
    // uint32_t result = raw_value & 0x0fffffff;
    // parse_result_data(channel, raw_value, &result);

    if (print)
    {
        printf("%ld\n",result);
        // printf("AY= %lx\n", L2[1]);
    }
    return 0;
}

esp_err_t LDC1612::set_conversion_time(uint8_t channel, uint16_t value){
    return i2c_driver.i2c_register_write_word(LDC_RCOUNT0+channel, value);
}

esp_err_t LDC1612::set_LC_stabilize_time(uint8_t channel){
    uint16_t value = 15;
    return i2c_driver.i2c_register_write_word(LDC_SETTLECOUNT0+channel, value);
};

esp_err_t LDC1612::set_conversion_offset(uint8_t channel, uint16_t value){
    return i2c_driver.i2c_register_write_word(LDC_OFFSET0+channel, value);
};

esp_err_t LDC1612::set_ERROR_CONFIG(uint16_t value) {
    return i2c_driver.i2c_register_write_word(LDC_ERROR_CONFIG, value);
}

esp_err_t LDC1612::set_mux_config(uint16_t value) {
    return i2c_driver.i2c_register_write_word(LDC_MUX_CONFIG, value);
}

esp_err_t LDC1612::set_driver_current(uint8_t channel, uint16_t value){
    return i2c_driver.i2c_register_write_word(LDC_DRIVE_CURRENT0+channel, value);
};

esp_err_t LDC1612::set_sensor_config(uint16_t value) {
    return i2c_driver.i2c_register_write_word(LDC_CONFIG, value);
}

esp_err_t LDC1612::set_FIN_FREF_DIV(uint8_t channel) {
    uint16_t value;
    uint16_t FIN_DIV, FREF_DIV;

    Fsensor[channel] = 1 / (2 * 3.14 * sqrt(inductance[channel] * capacitance[channel] * pow(10, -18))) * pow(10, -6);
    printf("fsensor =%.5f\n",Fsensor[channel]);


    FIN_DIV = (uint16_t)(Fsensor[channel] / 8.75 + 1);


    if (Fsensor[channel] * 4 < 40) {
        FREF_DIV = 2;
        Fref[channel] = 40 / 2;
    } else {
        FREF_DIV = 4;
        Fref[channel] = 40 / 4;
    }

    value = FIN_DIV << 12;
    value |= FREF_DIV;
    return i2c_driver.i2c_register_write_word(LDC_CLOCK_DIVIDERS0+channel, value);
}

void LDC1612::select_channel_to_convert(uint8_t channel, uint16_t* value) {
    switch (channel) {
        case 0: *value &= 0x3fff;
            break;
        case 1: *value &= 0x7fff;
            *value |= 0x4000;
            break;
        case 2: *value &= 0xbfff;
            *value |= 0x8000;
            break;
        case 3: *value |= 0xc000;
            break;
    }
}

void LDC1612::set_Rp(uint8_t channel, float n_kom) {
    resistance[channel] = n_kom;
}

void LDC1612::set_L(uint8_t channel, float n_uh) {
    inductance[channel] = n_uh;
}

void LDC1612::set_C(uint8_t channel, float n_pf) {
    capacitance[channel] = n_pf;
}


void LDC1612::set_Q_factor(uint8_t channel, float q) {
    Q_factor[channel] = q;
}

esp_err_t LDC1612::single_channel_config(uint8_t channel) {
    /*Set coil inductor parameter first.*/
    /*20 TURNS*/
    set_Rp(0, 15.727);
    set_L(0, 18.147);
    set_C(0, 100);
    set_Q_factor(0, 35.97);

    // /*25 TURNS*/
    // set_Rp(0,24.9);
    // set_L(0,53.95);
    // set_C(0,100);
    // set_Q_factor(0,32.57);

    /*36 TURNS,single layer*/

    // set_Rp(0,28.18);
    // set_L(0,18.56);
    // set_C(0,100);
    // set_Q_factor(0,43.25);

    /*40 TURNS*/
    // set_Rp(0,57.46);
    // set_L(0,85.44);
    // set_C(0,100);
    // set_Q_factor(0,40.7);


    if (set_FIN_FREF_DIV(0)) {
        return -1;
    }

    set_LC_stabilize_time(0);

    /*Set conversion interval time*/
    set_conversion_time(0, 0x04D3);

    /*Set driver current!*/
    set_driver_current(0, 0x6000);

    /*single conversion*/
    set_mux_config(0x20c);
    /*start channel 0*/
    uint16_t config = 0x1601;
    select_channel_to_convert(0, &config);
    set_sensor_config(config);
    return 0;
}

int LDC1612::mutiple_channel_config() {
    /*Set coil inductor parameter first.*/
    /*20 TURNS*/
    set_Rp(0, 15.727);
    set_L(0, 18.147);
    set_C(0, 100);
    set_Q_factor(0, 35.97);

    /*25 TURNS*/
    set_Rp(1, 15.727);
    set_L(1, 18.147);
    set_C(1, 100);
    set_Q_factor(1, 35.97);

    // /*36 TURNS single layer*/
    // set_Rp(0,28.18);
    // set_L(0,18.56);
    // set_C(0,100);
    // set_Q_factor(0,43.25);

    // /*40 TURNS*/
    // set_Rp(1,57.46);
    // set_L(1,85.44);
    // set_C(1,100);
    // set_Q_factor(0,40.7);


    if (set_FIN_FREF_DIV(0)) {
        return -1;
    }
    set_FIN_FREF_DIV(1);

    /* 16*38/Fref=30us ,wait 30us for LC sensor stabilize before initiation of a conversion.*/
    set_LC_stabilize_time(0);
    set_LC_stabilize_time(1);

    /*Set conversion interval time*/
    set_conversion_time(0, 0x0546);
    set_conversion_time(1, 0x0546);

    /*Set driver current!*/
    set_driver_current(0, 0xa000);
    set_driver_current(1, 0xa000);


    /*mutiple conversion*/
    set_mux_config(0x820c);
    //set_mux_config(0x20c);
    /*start channel 0*/
    set_sensor_config(0x1601);
    //uint16_t config=0x1601;
    //select_channel_to_convert(0,&config);
    return 0;
}

int LDC1612::parse_result_data(uint8_t channel, uint32_t raw_result, uint32_t* result) {
    uint8_t value = 0;
    *result = raw_result & 0x0fffffff;
    if (0xfffffff == *result) {
        printf("can't detect coil Coil Inductance!!!\n");
        *result = 0;
        return -1;
    }
    // else if(0==*result)
    // {
    //     printf("result is none!!!\n");
    // }
    value = raw_result >> 24;
    if (value & 0x80) {
        printf("channel %d",channel);
        printf(": ERR_UR-Under range error!!!\n");
    }
    if (value & 0x40) {
        printf("channel %d",channel);
        printf(": ERR_OR-Over range error!!!\n");
    }
    if (value & 0x20) {
        printf("channel %d",channel);
        printf(": ERR_WD-Watch dog timeout error!!!\n");
    }
    if (value & 0x10) {
        printf("channel %d",channel);
        printf(": ERR_AE-error!!!\n");
    }
    return 0;
}

int LDC1612::sensor_status_parse(uint16_t value) {
    uint16_t section = 0;
    section = value >> 14;
    switch (section) {
        case 0: printf("Channel 0 is source of flag or error.\n");
            break;
        case 1: printf("Channel 1 is source of flag or error.\n");
            break;
        /*Only support LDC1614*/
        case 2: printf("Channel 2 is source of flag or error.\n");
            break;
        case 3: printf("Channel 3 is source of flag or error.\n");
            break;
        default:
            break;
    }
    for (uint32_t i = 0; i < 6; i++) {
        if (value & (uint16_t)1 << (8 + i)) {
            printf(status_str[6 - i]);
        }
    }
    if (value & (1 << 6)) {
        printf(status_str[6]);
    }
    for (uint32_t i = 0; i < 4; i++) {
        if (value & (1 << i)) {
            printf(status_str[10 - i]);
        }
    }
    return 0;
}

int LDC1612::get_sensor_status() {
    uint16_t value = 0;
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_STATUS, buffer, 2));
    sensor_status_parse(value);
    return value;
}

esp_err_t LDC1612::reset()
{
    uint16_t _data = 0x8000;
    return i2c_driver.i2c_register_write_word(LDC_RESET_DEV, _data);
}

void LDC1612::check_who_am_i()
{
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_DEVICE_ID, buffer, reg_size));

    print_n_bytes("WHO_AM_I = ", buffer, reg_size);

    if ((buffer[0] << 8 | buffer[1]) != 0x3055)
        ESP_LOGW(LDC_TAG,"Device Address are not the same!");

    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(LDC_MANUFACTURER_ID, buffer, reg_size));

    print_n_bytes("manufacturer_id = ", buffer, reg_size);
}

void LDC1612::set_reg(uint8_t reg_addr, uint16_t data)
{
    ESP_ERROR_CHECK(i2c_driver.i2c_register_write_word(reg_addr, data));
}

void LDC1612::get_reg(uint8_t reg_addr, bool print, const char *_c)
{
    ESP_ERROR_CHECK(i2c_driver.i2c_register_read(reg_addr, buffer, reg_size));
    print_n_bytes(_c, buffer, reg_size);
}
