#ifndef MMA8451_H
#define MMA8451_H

#include "mbed.h"

// MMA8451 register definitions
#define MMA8451_WHO_AM_I           0x0D     // ID Register = 0b00011010
#define MMA8451_ADDRESS            0x1D << 1 // SA0 = 1 (default)
#define REG_CTRL_1                 0x2A     //
#define MMA8451_OUT_X_MSB          0x01     // 14-bit X Data (MSB)
#define MMA8451_OUT_X_LSB          0x02     // 14-bit X Data (LSB)
#define MMA8451_OUT_Y_MSB          0x03     // 14-bit Y Data (MSB)
#define MMA8451_OUT_Y_LSB          0x04     // 14-bit Y Data (LSB)
#define MMA8451_OUT_Z_MSB          0x05     // 14-bit Z Data (MSB)
#define MMA8451_OUT_Z_LSB          0x06     // 14-bit Z Data (LSB)
#define UINT14_MAX                 16383    // 2^14 -1 

class MMA8451 {
public:
    // Constructor
    MMA8451(I2C *i2c);

    // Initialize the accelerometer
    void initialize();

    // Get WHO_AM_I register value
    uint8_t getId();

    // Get accelerometer readings in m/s²
    float getX();
    float getY();
    float getZ();

    // Get accelerometer raw readings 
    int16_t getXraw();
    int16_t getYraw();
    int16_t getZraw();

private:
    // I2C object for communication
    I2C *i2c;

    // Read a single register
    bool readRegister(uint8_t reg, uint8_t *data);

    // Write to a register
    void writeRegister(uint8_t *data, int len);

    // Read axis data
    float readAxis(uint8_t reg);

    int16_t readAxisRaw(uint8_t reg);
};

#endif // MMA8451_H
