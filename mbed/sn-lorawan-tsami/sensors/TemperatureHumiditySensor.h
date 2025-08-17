#ifndef SI7021_H
#define SI7021_H

#include "mbed.h"

// Si7021 I2C address and command definitions
#define SI7021_ADDR           (0x40 << 1)   // Shifted left for 8-bit format
#define CMD_MEASURE_TEMP      0xE3          // Temperature command in hold master mode
#define CMD_MEASURE_HUMIDITY  0xE5          // Humidity command in hold master mode

class Si7021 {
public:
    // Constructor to initialize the sensor with I2C interface
    Si7021(I2C *i2c);

    // Public methods to read temperature and humidity
    float readTemperature();
    float readHumidity();
    uint16_t readData(char command);

private:
    I2C *i2c;  // Reference to the I2C interface
};

#endif // SI7021_H
