#include "TemperatureHumiditySensor.h"

// Constructor to initialize the sensor with the I2C interface
Si7021::Si7021(I2C *i2c) : i2c(i2c) {}

// Function to read raw 16-bit data from the sensor for a given command
uint16_t Si7021::readData(char command) {
    char data[2];  // Buffer for 2 bytes of data

    // Send the command to initiate measurement
    if (i2c->write(SI7021_ADDR, &command, 1) != 0) {
        return 0;  // Return 0 to indicate an error in writing
    }

    // Read the measurement data
    if (i2c->read(SI7021_ADDR, data, 2) != 0) {
        return 0;  // Return 0 to indicate an error in reading
    }

    // Combine the two bytes into a single 16-bit value
    return (data[0] << 8) | data[1];
}

// Function to read temperature in Celsius
// 14 bit default resolution in raw data (0.01 C)
float Si7021::readTemperature() {
    uint16_t raw_temperature = readData(CMD_MEASURE_TEMP);

    // If readData returned 0, there was an error
    if (raw_temperature == 0) {
        return -1.0f;  // Return -1 as an error indicator
    }

    // Convert raw data to Celsius as per datasheet
    return ((175.72f * raw_temperature) / 65536.0f) - 46.85f;
}

// Function to read relative humidity as a percentage
// 12 bit default resolution in raw data (0,025%)
float Si7021::readHumidity() {
    uint16_t raw_humidity = readData(CMD_MEASURE_HUMIDITY);

    // If readData returned 0, there was an error
    if (raw_humidity == 0) {
        return -1.0f;  // Return -1 as an error indicator
    }

    // Convert raw data to percentage as per datasheet
    return ((125.0f * raw_humidity) / 65536.0f) - 6.0f;
}
