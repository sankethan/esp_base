#include <inttypes.h>
#include <stdio.h>

const uint8_t AFS_SEL_2G  = 0<<3;
const uint8_t AFS_SEL_4G  = 1<<3;
const uint8_t AFS_SEL_8G  = 2<<3;
const uint8_t AFS_SEL_16G = 3<<3;

typedef struct  {
    uint8_t                 dev_address;
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    uint8_t                 AFS_SEL;
    int                     A_FS;         
    uint8_t                 data[14];
    int16_t                 A3[3];
    int                     calib_A3[3];
    int16_t                 L2[2];
}i2c_data;

void error_log (const char* _c){
    printf("ERROR: %s\n",_c);
}
void success_log (const char* _c){
    printf("Success: %s\n",_c);
}


#define MPU6050_RA_XG_OFFS_TC       0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_YG_OFFS_TC       0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_ZG_OFFS_TC       0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_X_FINE_GAIN      0x03 //[7:0] X_FINE_GAIN
#define MPU6050_RA_Y_FINE_GAIN      0x04 //[7:0] Y_FINE_GAIN
#define MPU6050_RA_Z_FINE_GAIN      0x05 //[7:0] Z_FINE_GAIN
#define MPU6050_RA_XA_OFFS_H        0x06 //[15:0] XA_OFFS
#define MPU6050_RA_XA_OFFS_L_TC     0x07
#define MPU6050_RA_YA_OFFS_H        0x08 //[15:0] YA_OFFS
#define MPU6050_RA_YA_OFFS_L_TC     0x09
#define MPU6050_RA_ZA_OFFS_H        0x0A //[15:0] ZA_OFFS
#define MPU6050_RA_ZA_OFFS_L_TC     0x0B
#define MPU6050_RA_SELF_TEST_X      0x0D //[7:5] XA_TEST[4-2], [4:0] XG_TEST[4-0]
#define MPU6050_RA_SELF_TEST_Y      0x0E //[7:5] YA_TEST[4-2], [4:0] YG_TEST[4-0]
#define MPU6050_RA_SELF_TEST_Z      0x0F //[7:5] ZA_TEST[4-2], [4:0] ZG_TEST[4-0]
#define MPU6050_RA_SELF_TEST_A      0x10 //[5:4] XA_TEST[1-0], [3:2] YA_TEST[1-0], [1:0] ZA_TEST[1-0]
#define MPU6050_RA_XG_OFFS_USRH     0x13 //[15:0] XG_OFFS_USR
#define MPU6050_RA_XG_OFFS_USRL     0x14
#define MPU6050_RA_YG_OFFS_USRH     0x15 //[15:0] YG_OFFS_USR
#define MPU6050_RA_YG_OFFS_USRL     0x16
#define MPU6050_RA_ZG_OFFS_USRH     0x17 //[15:0] ZG_OFFS_USR
#define MPU6050_RA_ZG_OFFS_USRL     0x18
#define MPU6050_RA_SMPLRT_DIV       0x19
#define MPU6050_RA_CONFIG           0x1A
#define MPU6050_RA_GYRO_CONFIG      0x1B
#define MPU6050_RA_ACCEL_CONFIG     0x1C
#define MPU6050_RA_FF_THR           0x1D
#define MPU6050_RA_FF_DUR           0x1E
#define MPU6050_RA_MOT_THR          0x1F
#define MPU6050_RA_MOT_DUR          0x20
#define MPU6050_RA_ZRMOT_THR        0x21
#define MPU6050_RA_ZRMOT_DUR        0x22
#define MPU6050_RA_FIFO_EN          0x23
#define MPU6050_RA_I2C_MST_CTRL     0x24
#define MPU6050_RA_I2C_SLV0_ADDR    0x25
#define MPU6050_RA_I2C_SLV0_REG     0x26
#define MPU6050_RA_I2C_SLV0_CTRL    0x27
#define MPU6050_RA_I2C_SLV1_ADDR    0x28
#define MPU6050_RA_I2C_SLV1_REG     0x29
#define MPU6050_RA_I2C_SLV1_CTRL    0x2A
#define MPU6050_RA_I2C_SLV2_ADDR    0x2B
#define MPU6050_RA_I2C_SLV2_REG     0x2C
#define MPU6050_RA_I2C_SLV2_CTRL    0x2D
#define MPU6050_RA_I2C_SLV3_ADDR    0x2E
#define MPU6050_RA_I2C_SLV3_REG     0x2F
#define MPU6050_RA_I2C_SLV3_CTRL    0x30
#define MPU6050_RA_I2C_SLV4_ADDR    0x31
#define MPU6050_RA_I2C_SLV4_REG     0x32
#define MPU6050_RA_I2C_SLV4_DO      0x33
#define MPU6050_RA_I2C_SLV4_CTRL    0x34
#define MPU6050_RA_I2C_SLV4_DI      0x35
#define MPU6050_RA_I2C_MST_STATUS   0x36
#define MPU6050_RA_INT_PIN_CFG      0x37
#define MPU6050_RA_INT_ENABLE       0x38
#define MPU6050_RA_DMP_INT_STATUS   0x39
#define MPU6050_RA_INT_STATUS       0x3A
#define MPU6050_RA_ACCEL_XOUT_H     0x3B
#define MPU6050_RA_ACCEL_XOUT_L     0x3C
#define MPU6050_RA_ACCEL_YOUT_H     0x3D
#define MPU6050_RA_ACCEL_YOUT_L     0x3E
#define MPU6050_RA_ACCEL_ZOUT_H     0x3F
#define MPU6050_RA_ACCEL_ZOUT_L     0x40
#define MPU6050_RA_TEMP_OUT_H       0x41
#define MPU6050_RA_TEMP_OUT_L       0x42
#define MPU6050_RA_GYRO_XOUT_H      0x43
#define MPU6050_RA_GYRO_XOUT_L      0x44
#define MPU6050_RA_GYRO_YOUT_H      0x45
#define MPU6050_RA_GYRO_YOUT_L      0x46
#define MPU6050_RA_GYRO_ZOUT_H      0x47
#define MPU6050_RA_GYRO_ZOUT_L      0x48
#define MPU6050_RA_EXT_SENS_DATA_00 0x49
#define MPU6050_RA_EXT_SENS_DATA_01 0x4A
#define MPU6050_RA_EXT_SENS_DATA_02 0x4B
#define MPU6050_RA_EXT_SENS_DATA_03 0x4C
#define MPU6050_RA_EXT_SENS_DATA_04 0x4D
#define MPU6050_RA_EXT_SENS_DATA_05 0x4E
#define MPU6050_RA_EXT_SENS_DATA_06 0x4F
#define MPU6050_RA_EXT_SENS_DATA_07 0x50
#define MPU6050_RA_EXT_SENS_DATA_08 0x51
#define MPU6050_RA_EXT_SENS_DATA_09 0x52
#define MPU6050_RA_EXT_SENS_DATA_10 0x53
#define MPU6050_RA_EXT_SENS_DATA_11 0x54
#define MPU6050_RA_EXT_SENS_DATA_12 0x55
#define MPU6050_RA_EXT_SENS_DATA_13 0x56
#define MPU6050_RA_EXT_SENS_DATA_14 0x57
#define MPU6050_RA_EXT_SENS_DATA_15 0x58
#define MPU6050_RA_EXT_SENS_DATA_16 0x59
#define MPU6050_RA_EXT_SENS_DATA_17 0x5A
#define MPU6050_RA_EXT_SENS_DATA_18 0x5B
#define MPU6050_RA_EXT_SENS_DATA_19 0x5C
#define MPU6050_RA_EXT_SENS_DATA_20 0x5D
#define MPU6050_RA_EXT_SENS_DATA_21 0x5E
#define MPU6050_RA_EXT_SENS_DATA_22 0x5F
#define MPU6050_RA_EXT_SENS_DATA_23 0x60
#define MPU6050_RA_MOT_DETECT_STATUS    0x61
#define MPU6050_RA_I2C_SLV0_DO      0x63
#define MPU6050_RA_I2C_SLV1_DO      0x64
#define MPU6050_RA_I2C_SLV2_DO      0x65
#define MPU6050_RA_I2C_SLV3_DO      0x66
#define MPU6050_RA_I2C_MST_DELAY_CTRL   0x67
#define MPU6050_RA_SIGNAL_PATH_RESET    0x68
#define MPU6050_RA_MOT_DETECT_CTRL      0x69
#define MPU6050_RA_USER_CTRL        0x6A
#define MPU6050_RA_PWR_MGMT_1       0x6B
#define MPU6050_RA_PWR_MGMT_2       0x6C
#define MPU6050_RA_BANK_SEL         0x6D
#define MPU6050_RA_MEM_START_ADDR   0x6E
#define MPU6050_RA_MEM_R_W          0x6F
#define MPU6050_RA_DMP_CFG_1        0x70
#define MPU6050_RA_DMP_CFG_2        0x71
#define MPU6050_RA_FIFO_COUNTH      0x72
#define MPU6050_RA_FIFO_COUNTL      0x73
#define MPU6050_RA_FIFO_R_W         0x74
#define MPU6050_RA_WHO_AM_I         0x75





#define LDC_DATA0_MSB 0x00   // 0x0000 Channel 0 MSB Conversion Result and Error Status
#define LDC_DATA0_LSB 0x01   // 0x0000 Channel 0 LSB Conversion Result. Must be read after Register address
#define LDC_DATA1_MSB 0x02   // 0x0000 Channel 1 MSB Conversion Result and Error Status.
#define LDC_DATA1_LSB 0x03   // 0x0000 Channel 1 LSB Conversion Result. Must be read after Register address
#define LDC_DATA2_MSB 0x04   // 0x0000 Channel 2 MSB Conversion Result and Error Status. (LDC1614 only)
#define LDC_DATA2_LSB 0x05   // 0x0000 Channel 2 LSB Conversion Result. Must be read after Register address
#define LDC_DATA3_MSB 0x06   // 0x0000 Channel 3 MSB Conversion Result and Error Status. (LDC1614 only)
#define LDC_DATA3_LSB 0x07   // 0x0000 Channel 3 LSB Conversion Result. Must be read after Register address
#define LDC_RCOUNT0 0x08   // 0x0080 Reference Count setting for Channel 0
#define LDC_RCOUNT1 0x09   // 0x0080 Reference Count setting for Channel 1
#define LDC_RCOUNT2 0x0A   // 0x0080 Reference Count setting for Channel 2. (LDC1614 only)
#define LDC_RCOUNT3 0x0B   // 0x0080 Reference Count setting for Channel 3.(LDC1614 only)
#define LDC_OFFSET0 0x0C   // 0x0000 Offset value for Channel 0
#define LDC_OFFSET1 0x0D   // 0x0000 Offset value for Channel 1
#define LDC_OFFSET2 0x0E   // 0x0000 Offset value for Channel 2 (LDC1614 only)
#define LDC_OFFSET3 0x0F   // 0x0000 Offset value for Channel 3 (LDC1614 only)
#define LDC_SETTLECOUNT0 0x10   // 0x0000 Channel 0 Settling Reference Count
#define LDC_SETTLECOUNT1 0x11   // 0x0000 Channel 1 Settling Reference Count
#define LDC_SETTLECOUNT2 0x12   // 0x0000 Channel 2 Settling Reference Count (LDC1614 only)
#define LDC_SETTLECOUNT3 0x13   // 0x0000 Channel 3 Settling Reference Count (LDC1614 only)
#define LDC_CLOCK_DIVIDERS0 0x14   // 0x0000 Reference and Sensor Divider settings for Channel 0
#define LDC_CLOCK_DIVIDERS1 0x15   // 0x0000 Reference and Sensor Divider settings for Channel 1
#define LDC_CLOCK_DIVIDERS2 0x16   // 0x0000 Reference and Sensor Divider settings for Channel 2 (LDC1614 only)
#define LDC_CLOCK_DIVIDERS3 0x17   // 0x0000 Reference and Sensor Divider settings for Channel 3 (LDC1614 only)
#define LDC_STATUS 0x18   // 0x0000 Device Status Report
#define LDC_ERROR_CONFIG 0x19   // 0x0000 Error Reporting Configuration
#define LDC_CONFIG 0x1A   // 0x2801 Conversion Configuration
#define LDC_MUX_CONFIG 0x1B   // 0x020F Channel Multiplexing Configuration
#define LDC_RESET_DEV 0x1C   // 0x0000 Reset Device
#define LDC_DRIVE_CURRENT0 0x1E   // 0x0000 Channel 0 sensor current drive configuration
#define LDC_DRIVE_CURRENT1 0x1F   // 0x0000 Channel 1 sensor current drive configuration
#define LDC_DRIVE_CURRENT2 0x20   // 0x0000 Channel 2 sensor current drive configuration (LDC1614 only)
#define LDC_DRIVE_CURRENT3 0x21   // 0x0000 Channel 3 sensor current drive configuration (LDC1614 only)
#define LDC_MANUFACTURER_ID 0x7E   // 0x5449 Manufacturer ID
#define LDC_DEVICE_ID 0x7F   // 0x3055 Device ID
