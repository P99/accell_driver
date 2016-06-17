#ifndef _LIS3DH_ACC_H_
#define _LIS3DH_ACC_H_

#define I2C_ADDR_PRIMARY     (0x18)
#define I2C_ADDR_SECONDARY   (0x19)
#define I2C_AUTO_INCREMENT   (0x80) /* MSB on 8 bits address */
#define WHO_AM_I_OUTPUT      (0x33)


/* LIS3DH registers */
enum lis3dh_registers {
    STATUS_REG_AUX = 0x07,
    OUT_ADC1_L,
    OUT_ADC1_H,
    OUT_ADC2_L,
    OUT_ADC2_H,
    OUT_ADC3_L,
    OUT_ADC3_H,
    INT_COUNTER_REG,
    WHO_AM_I,
    TEMP_CFG_REG = 0x1f,
    CTRL_REG1,
    CTRL_REG2,
    CTRL_REG3,
    CTRL_REG4,
    CTRL_REG5,
    CTRL_REG6,
    REFERENCE,
    STATUS_REG2,
    OUT_X_L,
    OUT_X_H,
    OUT_Y_L,
    OUT_Y_H,
    OUT_Z_L,
    OUT_Z_H,
    FIFO_CTRL_REG,
    FIFO_SRC_REG,
    INT1_CFG,
    INT1_SOURCE,
    INT1_THS,
    INT1_DURATION,
    CLICK_CFG = 0x38,
    CLICK_SRC,
    CLICK_THS,
    TIME_LIMIT,
    TIME_LATENCY,
    TIME_WINDOW
};

#endif /*_LIS3DH_ACC_H_ */
