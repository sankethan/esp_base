#include <stdio.h>
#include <inttypes.h>
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/i2c_master.h"
#include "main.h"
#include <math.h>
#include "freertos/FreeRTOS.h"

static esp_err_t i2c_register_read(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, 1000);
}

static esp_err_t i2c_register_write_byte(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), 1000);
}
void print_n_bytes(uint8_t *arr, int n)
{
    int i = 0;
    while (i < n)
    {

        printf("0x%X ", arr[i]);
        i++;
    }
    printf("\n");
}

static esp_err_t i2c_register_write_word(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint16_t data)
{
    
    uint8_t write_buf[3] = {reg_addr,(data&0xFF00)>>8,data&0x00FF};
    // print_n_bytes(write_buf,3);
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), 1000);
}
void print_header()
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
}

i2c_data i2c_init(uint8_t addr)
{

    i2c_master_bus_handle_t bus_handle;
    i2c_master_bus_config_t i2c_mst_config = {
        .i2c_port = 1,
        .sda_io_num = GPIO_NUM_4,
        .scl_io_num = GPIO_NUM_2,
        .clk_source = I2C_CLK_SRC_DEFAULT,

        .glitch_ignore_cnt = 7,

        .flags.enable_internal_pullup = 1};
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_master_dev_handle_t dev_handle;
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    i2c_data i2c_data = {
        .bus_handle = bus_handle,
        .dev_handle = dev_handle,
        .AFS_SEL = AFS_SEL_2G,
        .data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .dev_address = addr};
    
    return i2c_data;
}


void check_who_am_i(i2c_data i2c_data){
    ESP_ERROR_CHECK(i2c_register_read(i2c_data.dev_handle, 0x75, i2c_data.data, 1));
    printf("WHO_AM_I = %X(%d)\n", i2c_data.data[0], i2c_data.data[0]);
    if (i2c_data.data[0] != i2c_data.dev_address)
        error_log("Device Address are not the same!");
    else
        success_log("Device Address matched!");
    // Reset MSP
    ESP_ERROR_CHECK(i2c_register_write_byte(i2c_data.dev_handle, MPU6050_RA_SIGNAL_PATH_RESET, 0x07));
}

void get_a3(i2c_data *i2c_data, bool print)
{

    ESP_ERROR_CHECK(i2c_register_read(i2c_data->dev_handle, MPU6050_RA_ACCEL_XOUT_H, i2c_data->data, 2));
    i2c_data->A3[0] = ((uint16_t)i2c_data->data[0] << 8) | i2c_data->data[1];

    ESP_ERROR_CHECK(i2c_register_read(i2c_data->dev_handle, MPU6050_RA_ACCEL_YOUT_H, i2c_data->data, 2));
    i2c_data->A3[1] = ((uint16_t)i2c_data->data[0] << 8) | i2c_data->data[1];

    ESP_ERROR_CHECK(i2c_register_read(i2c_data->dev_handle, MPU6050_RA_ACCEL_ZOUT_H, i2c_data->data, 2));
    i2c_data->A3[2] = ((uint16_t)i2c_data->data[0] << 8) | i2c_data->data[1];

    if (print)
    {
        printf("AX= %04.02f m/s^2\n", ((float)i2c_data->A3[0] * 9.8) / i2c_data->A_FS);
        printf("AY= %04.02f m/s^2\n", ((float)i2c_data->A3[1] * 9.8) / i2c_data->A_FS);
        printf("AZ= %04.02f m/s^2\n", ((float)i2c_data->A3[2] * 9.8) / i2c_data->A_FS);
    }
}

void get_l2(i2c_data *i2c_data, bool print)
{

    ESP_ERROR_CHECK(i2c_register_read(i2c_data->dev_handle, LDC_DATA0_MSB, i2c_data->data, 2));
    i2c_data->L2[0] = ((uint16_t)i2c_data->data[0] << 8) | i2c_data->data[1];

    ESP_ERROR_CHECK(i2c_register_read(i2c_data->dev_handle, LDC_DATA1_MSB, i2c_data->data, 2));
    i2c_data->L2[1] = ((uint16_t)i2c_data->data[0] << 8) | i2c_data->data[1];

    

    if (print)
    {
        printf("AX= %x\n", i2c_data->L2[0] );
        printf("AY= %x\n", i2c_data->L2[1] );
    }
}
void Calibrate(i2c_data *i2c_data_p)
{
    unsigned int count1 = 0;
    int sstatex = 0.0f, sstatey = 0.0f, sstatez= 0.0f;
    do
    {
        get_a3(i2c_data_p, false);
        sstatex = sstatex + i2c_data_p->A3[0];
        sstatey = sstatey + i2c_data_p->A3[1];
        sstatez = sstatez + i2c_data_p->A3[2];
        count1++;
        vTaskDelay(configTICK_RATE_HZ/40);//(1/n)s
    } while (count1 != 0x0400); // 1024 times
    i2c_data_p->calib_A3[0] = sstatex >> 10; // division between 1024
    i2c_data_p->calib_A3[1] = sstatey >> 10;
    i2c_data_p->calib_A3[2] = sstatez >> 10;
    printf("meanx= %0.2f\n", (float)i2c_data_p->calib_A3[0]*9.8 / i2c_data_p->A_FS);
    printf("meany= %0.2f\n", (float)i2c_data_p->calib_A3[1]*9.8 / i2c_data_p->A_FS);
    printf("meanz= %0.2f\n", (float)i2c_data_p->calib_A3[2]*9.8 / i2c_data_p->A_FS);
}

void set_msp_fs_range(i2c_data *i2c_data_p, const uint8_t AFS_SEL_X)
{
    ESP_ERROR_CHECK(i2c_register_write_byte(i2c_data_p->dev_handle, MPU6050_RA_ACCEL_CONFIG, AFS_SEL_X));
    ESP_ERROR_CHECK(i2c_register_read(i2c_data_p->dev_handle, MPU6050_RA_ACCEL_CONFIG, i2c_data_p->data, 2));
    printf("ACCEL_CONFIG = %X,%X \n", i2c_data_p->data[1], i2c_data_p->data[0]);

    int fsr_sel = ((i2c_data_p->data[0] & 0x18) >> 3);
    double fsr_gs = pow(2, fsr_sel + 1);

    i2c_data_p->A_FS = (16384 * 2) / fsr_gs;

    printf("A_FS =%d LSBs/g\n", i2c_data_p->A_FS);
    printf("Full Scale Range = %.0fg\n", fsr_gs);
    if (fsr_sel != AFS_SEL_X)
        error_log("fsr is not set!");
    else
        success_log("fsr is set!");
}

void set_pwr_regs(i2c_data *i2c_data_p, uint8_t pm_1, uint8_t pm_2)
{
    ESP_ERROR_CHECK(i2c_register_write_byte(i2c_data_p->dev_handle, MPU6050_RA_PWR_MGMT_1, pm_1));
    ESP_ERROR_CHECK(i2c_register_read(i2c_data_p->dev_handle, MPU6050_RA_PWR_MGMT_1, i2c_data_p->data, 1));
    printf("PWR_MGMT_1 = %X \n", i2c_data_p->data[0]);

    if (i2c_data_p->data[0] != pm_1)
        error_log("pwr mgt 1 is not set!");
    else
        success_log("pwr mgt 1 is set!");

    ESP_ERROR_CHECK(i2c_register_write_byte(i2c_data_p->dev_handle, MPU6050_RA_PWR_MGMT_2, pm_2));
    ESP_ERROR_CHECK(i2c_register_read(i2c_data_p->dev_handle, MPU6050_RA_PWR_MGMT_2, i2c_data_p->data, 1));
    printf("PWR_MGMT_2 = %X \n", i2c_data_p->data[0]);

    if (i2c_data_p->data[0] != pm_2)
        error_log("pwr mgt 2 is not set!");
    else
        success_log("pwr mgt 2 is set!");
}
// void app_main(void)
// {
    // print_header();
    // i2c_data i2c_data = i2c_init(0x68);
    // check_who_am_i(i2c_data);
    // // Setting Full Scale Range To 2G
    // set_msp_fs_range(&i2c_data, AFS_SEL_2G);

    // set_pwr_regs(&i2c_data, 0x00, 0xC0);

    // Calibrate(&i2c_data);

    // int i = 1000;
    // double accelerationx[2] = {0,0}, accelerationy[2]= {0,0};
    // double velocityx[2]= {0,0}, velocityy[2]= {0,0};
    // signed long positionX[2]= {0,0};
    // signed long positionY[2]= {0,0};
    // int loop_s = configTICK_RATE_HZ/10;
    // while (i--)
    // {
    //     get_a3(&i2c_data, false);   
    //     accelerationx[1] = (i2c_data.A3[2] - i2c_data.calib_A3[2]);//*9.8)/i2c_data.A_FS; // eliminating zero reference
    //     accelerationy[1] = (i2c_data.A3[1] - i2c_data.calib_A3[1]);//*9.8)/i2c_data.A_FS; // to obtain positive and negative
    //     if ((accelerationx[1] <= 200) && (accelerationx[1] >= -200))    // Discrimination window applied
    //     {
    //         accelerationx[1] = 0;
    //     } // to the X axis acceleration
    //     if ((accelerationy[1] <= 200) && (accelerationy[1] >= -200))
    //     {
    //         accelerationy[1] = 0;
    //     }
    //     printf("ax = %0.5f , ay = %0.5f\n",(accelerationx[1]*9.8)/i2c_data.A_FS,(accelerationy[1])/i2c_data.A_FS);
    //     velocityx[1] = velocityx[0] + (accelerationx[0] + (((accelerationx[1] - accelerationx[0]) /2))*(loop_s/100));
    //     velocityy[1] = velocityy[0] + (accelerationy[0] + (((accelerationy[1] - accelerationy[0]) /2))*(loop_s/100));
    //     printf("vx = %0.5f , vy = %0.5f\n",(velocityx[1]),(velocityy[1]));

    //     // // second Y integration:
    //     // positionX[1] = positionX[0] + velocityx[0] + (((velocityx[1] - velocityx[0]) >> 1)*0.000050);
    //     // positionY[1] = positionY[0] + velocityy[0] + (((velocityy[1] - velocityy[0]) >> 1)*0.000050);

    //     accelerationx[0] = accelerationx[1]; // The current acceleration value must be sent
    //     accelerationy[0] = accelerationy[1]; // variable in order to introduce the new

    //     velocityx[0] = velocityx[1]; //Same done for the velocity variable
    //     velocityy[0] = velocityy[1];

    //     // printf("%ld , %ld\n",positionX[1],positionY[1]);
    //     // positionX[0] = positionX[1]; //actual position data must be sent to the
    //     // positionY[0] = positionY[1]; //previous position
    //     vTaskDelay(loop_s); // 100ms
    // }

    // fflush(stdout);
// }

void check_who_am_i_LDC(i2c_data *i2c_data_p){
    ESP_ERROR_CHECK(i2c_register_read(i2c_data_p->dev_handle, 0x7F, i2c_data_p->data, 2));
    printf("WHO_AM_I = %X %X\n", i2c_data_p->data[0], i2c_data_p->data[1]);
    if ((i2c_data_p->data[0]<<8|i2c_data_p->data[1]) != 0x3055 )
        error_log("Device Address are not the same!");
    else
        success_log("Device Address matched!");
}


void reset_LDC(i2c_data *i2c_data_p){
    ESP_ERROR_CHECK(i2c_register_write_word(i2c_data_p->dev_handle, LDC_RESET_DEV, 0x8000));
}

void start_LDC(i2c_data * i2c_data_p){
    ESP_ERROR_CHECK(i2c_register_write_word(i2c_data_p->dev_handle, LDC_CONFIG, 0x0001));
    ESP_ERROR_CHECK(i2c_register_read(i2c_data_p->dev_handle, LDC_CONFIG, i2c_data_p->data, 2));
    printf("CONFIG = %X %X\n", i2c_data_p->data[0], i2c_data_p->data[1]);
}
void app_main(void)
{
    print_header();
    i2c_data i2c_data = i2c_init(0x2B);
    check_who_am_i_LDC(&i2c_data);
    // Reset LDC
    reset_LDC(&i2c_data);
    
    // Start LDC
    start_LDC(&i2c_data);
    

    
    while(true){
        get_l2(&i2c_data,true);
        vTaskDelay(configTICK_RATE_HZ/1); // 50us
    }
    

    fflush(stdout);
}