#include <inttypes.h>
#include <stdio.h>
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/i2c_master.h"
// #include "main.h"
#include <math.h>
#include "freertos/FreeRTOS.h"

#define LDC_DATA0_MSB 0x00       // 0x0000 Channel 0 MSB Conversion Result and Error Status
#define LDC_DATA0_LSB 0x01       // 0x0000 Channel 0 LSB Conversion Result. Must be read after Register address
#define LDC_DATA1_MSB 0x02       // 0x0000 Channel 1 MSB Conversion Result and Error Status.
#define LDC_DATA1_LSB 0x03       // 0x0000 Channel 1 LSB Conversion Result. Must be read after Register address
#define LDC_DATA2_MSB 0x04       // 0x0000 Channel 2 MSB Conversion Result and Error Status. (LDC1614 only)
#define LDC_DATA2_LSB 0x05       // 0x0000 Channel 2 LSB Conversion Result. Must be read after Register address
#define LDC_DATA3_MSB 0x06       // 0x0000 Channel 3 MSB Conversion Result and Error Status. (LDC1614 only)
#define LDC_DATA3_LSB 0x07       // 0x0000 Channel 3 LSB Conversion Result. Must be read after Register address
#define LDC_RCOUNT0 0x08         // 0x0080 Reference Count setting for Channel 0
#define LDC_RCOUNT1 0x09         // 0x0080 Reference Count setting for Channel 1
#define LDC_RCOUNT2 0x0A         // 0x0080 Reference Count setting for Channel 2. (LDC1614 only)
#define LDC_RCOUNT3 0x0B         // 0x0080 Reference Count setting for Channel 3.(LDC1614 only)
#define LDC_OFFSET0 0x0C         // 0x0000 Offset value for Channel 0
#define LDC_OFFSET1 0x0D         // 0x0000 Offset value for Channel 1
#define LDC_OFFSET2 0x0E         // 0x0000 Offset value for Channel 2 (LDC1614 only)
#define LDC_OFFSET3 0x0F         // 0x0000 Offset value for Channel 3 (LDC1614 only)
#define LDC_SETTLECOUNT0 0x10    // 0x0000 Channel 0 Settling Reference Count
#define LDC_SETTLECOUNT1 0x11    // 0x0000 Channel 1 Settling Reference Count
#define LDC_SETTLECOUNT2 0x12    // 0x0000 Channel 2 Settling Reference Count (LDC1614 only)
#define LDC_SETTLECOUNT3 0x13    // 0x0000 Channel 3 Settling Reference Count (LDC1614 only)
#define LDC_CLOCK_DIVIDERS0 0x14 // 0x0000 Reference and Sensor Divider settings for Channel 0
#define LDC_CLOCK_DIVIDERS1 0x15 // 0x0000 Reference and Sensor Divider settings for Channel 1
#define LDC_CLOCK_DIVIDERS2 0x16 // 0x0000 Reference and Sensor Divider settings for Channel 2 (LDC1614 only)
#define LDC_CLOCK_DIVIDERS3 0x17 // 0x0000 Reference and Sensor Divider settings for Channel 3 (LDC1614 only)
#define LDC_STATUS 0x18          // 0x0000 Device Status Report
#define LDC_ERROR_CONFIG 0x19    // 0x0000 Error Reporting Configuration
#define LDC_CONFIG 0x1A          // 0x2801 Conversion Configuration
#define LDC_MUX_CONFIG 0x1B      // 0x020F Channel Multiplexing Configuration
#define LDC_RESET_DEV 0x1C       // 0x0000 Reset Device
#define LDC_DRIVE_CURRENT0 0x1E  // 0x0000 Channel 0 sensor current drive configuration
#define LDC_DRIVE_CURRENT1 0x1F  // 0x0000 Channel 1 sensor current drive configuration
#define LDC_DRIVE_CURRENT2 0x20  // 0x0000 Channel 2 sensor current drive configuration (LDC1614 only)
#define LDC_DRIVE_CURRENT3 0x21  // 0x0000 Channel 3 sensor current drive configuration (LDC1614 only)
#define LDC_MANUFACTURER_ID 0x7E // 0x5449 Manufacturer ID
#define LDC_DEVICE_ID 0x7F       // 0x3055 Device ID

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
};

enum LEVEL
{
    LOW,
    MEDIUM,
    HIGH
};

class Log
{
private:
    enum LEVEL current_lvl;

public:
    Log(LEVEL level)
    {
        current_lvl = level;
    }
    void error_log(const char *_c)
    {
        printf("ERROR: %s\n", _c);
    }
    void success_log(const char *_c)
    {
        printf("Success: %s\n", _c);
    }
    void log(const char *_c, LEVEL c_lvl)
    {
        if (c_lvl >= current_lvl) printf("%s\n", _c);
    }
};

class LDC1612
{
private:
    i2c_base i2c_driver;
    int reg_size;
    uint8_t buffer[20];
    Log log;
    uint16_t L2[2];

public:
    void check_who_am_i();
    
    LDC1612(uint8_t addr, int reg_size) ;

    void get_l2(bool print);
    void reset();
    void print_n_bytes(const char* _c,uint8_t *arr, int n);
};

