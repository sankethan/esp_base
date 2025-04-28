/*
    Seeed_LDC1612.h
    Driver for LDC1612

    Website    : 
    Author     : Sankethan
    Create Time: April 2025
    Change Log :
*/

#ifndef LDC1612_H
#define LDC1612_H

#include "i2c_base.h"
#include "esp_log.h"



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
class LDC1612
{
private:
    i2c_base i2c_driver;
    const int reg_size = 2;
    uint8_t buffer[20];
    
    const char *LDC_TAG ="LDC1612";
    const char *status_str[11] = {"conversion under range error", "conversion over range error",
                                  "watch dog timeout error", "Amplitude High Error",
                                  "Amplitude Low Error", "Zero Count Error",
                                  "Data Ready", "unread conversion is present for channel 0",
                                  " unread conversion is present for Channel 1.",
                                  "unread conversion ispresent for Channel 2.",
                                  "unread conversion is present for Channel 3."};
    float resistance[2];
    float inductance[2];
    float capacitance[2];
    float Fref[2];
    float Fsensor[2];
    float Q_factor[2];
    int parse_result_data(uint8_t channel, uint32_t raw_result, uint32_t* result);
    int sensor_status_parse(uint16_t value);
public:
    uint32_t L2[2];
    LDC1612(uint8_t addr);
    void configure();
    void get_l2(bool print);
    void get_l1(bool print);
    int get_channel_result(uint8_t channel, bool print);
    void check_who_am_i();
    void set_reg(uint8_t reg_addr, uint16_t data);
    void get_reg(uint8_t reg_addr, bool print, const char *_c);

    esp_err_t set_conversion_time(uint8_t channel, uint16_t value);
    esp_err_t set_LC_stabilize_time(uint8_t channel);
    esp_err_t set_conversion_offset(uint8_t channel, uint16_t value);
    esp_err_t set_ERROR_CONFIG(uint16_t value);
    esp_err_t set_mux_config(uint16_t value);
    esp_err_t reset();
    esp_err_t set_driver_current(uint8_t channel, uint16_t value);
    esp_err_t set_FIN_FREF_DIV(uint8_t channel);
    esp_err_t set_sensor_config(uint16_t value);

    int single_channel_config(uint8_t channel);
    int mutiple_channel_config();
    void select_channel_to_convert(uint8_t channel, uint16_t *value);
    int get_sensor_status();
    

    void set_Rp(uint8_t channel, float n_kom);
    void set_L(uint8_t channel, float n_uh);
    void set_C(uint8_t channel, float n_pf);
    void set_Q_factor(uint8_t channel, float q);
};
#endif