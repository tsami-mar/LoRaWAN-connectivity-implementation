#include "mbed.h"
#include "Accelerometer.h"

// Constructor: initialize I2C with given pins
MMA8451::MMA8451(I2C *i2c) : i2c(i2c) {
    initialize();
}

// Initialize accelerometer: set control register
void MMA8451::initialize() {
    uint8_t data[2] = {REG_CTRL_1, 0x01}; // Set active mode, normal reading mode (14 bit)
    writeRegister(data, 2);
}

// Get WHO_AM_I register value
uint8_t MMA8451::getId() {
    uint8_t whoAmI = 0;
    readRegister(MMA8451_WHO_AM_I, &whoAmI);
    return whoAmI;
}

// Get X-axis acceleration in m/s²
float MMA8451::getX() {
    return readAxis(MMA8451_OUT_X_MSB);
}

// Get Y-axis acceleration in m/s²
float MMA8451::getY() {
    return readAxis(MMA8451_OUT_Y_MSB);
}

// Get Z-axis acceleration in m/s²
float MMA8451::getZ() {
    return readAxis(MMA8451_OUT_Z_MSB);
}

// Read a single register
bool MMA8451::readRegister(uint8_t reg, uint8_t *data) {
    int status = i2c->write(MMA8451_ADDRESS, (char*)&reg, 1, true);
    status |= i2c->read(MMA8451_ADDRESS, (char*)data, 1, false);
    return (status == 0);
}

// Write to a register
void MMA8451::writeRegister(uint8_t *data, int len) {
    i2c->write(MMA8451_ADDRESS, (char*)data, len);
}

// Read and process axis data
// In 2g mode the sensitivity is 4096 counts/g (default)
float MMA8451::readAxis(uint8_t reg) {
    char buf[2] = {0};

    // Request data from the sensor
    i2c->write(MMA8451_ADDRESS, (char*)&reg, 1, true);
    // When performing a multi-byte read or burst read, the MMA8451Q automatically increments the received register address commands after a read command is received.
    i2c->read(MMA8451_ADDRESS, buf, 2, false);

    // Combine the two bytes and shift to get 14-bit signed data
    int16_t axisData = (static_cast<int16_t>(buf[0]) << 8 | buf[1]) >> 2;

    // Adjust for signed 14-bit range if needed
    if (axisData > UINT14_MAX / 2) {
        axisData -= UINT14_MAX + 1;
    }

    // Convert to g (divide by sensitivity- counts per g), then to m/s² (1g ≈ 9.81 m/s²)
    return (static_cast<float>(axisData) / 4096.0) * 9.81;
}

// Read raw axis data
int16_t MMA8451::readAxisRaw(uint8_t reg) {
    char buf[2] = {0};

    // Request data from the sensor
    i2c->write(MMA8451_ADDRESS, (char*)&reg, 1, true);
    // When performing a multi-byte read or burst read, the MMA8451Q automatically increments the received register address commands after a read command is received.
    i2c->read(MMA8451_ADDRESS, buf, 2, false);

    // Combine the two bytes and shift to get 14-bit signed data
    int16_t axisData = (static_cast<int16_t>(buf[0]) << 8 | buf[1]) >> 2;

    return axisData;
}

// Get X-axis acceleration in m/s²
int16_t MMA8451::getXraw() {
    return readAxisRaw(MMA8451_OUT_X_MSB);
}

// Get Y-axis acceleration in m/s²
int16_t MMA8451::getYraw() {
    return readAxisRaw(MMA8451_OUT_Y_MSB);
}

// Get Z-axis acceleration in m/s²
int16_t MMA8451::getZraw() {
    return readAxisRaw(MMA8451_OUT_Z_MSB);
}
