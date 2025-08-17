#include "mbed.h"
#include "RGBsensor.h"

// Constructor: initialize I2C with specified SDA and SCL pins
TCS34725::TCS34725(I2C *i2c) : i2c(i2c) {
    initialize();
}

// Initialize the TCS34725 sensor
void TCS34725::initialize() {
    // Power ON the sensor (activate the internal oscillator to permit the timers and ADC channels to operate)
    writeRegister(TCS34725_ENABLE, TCS34725_ENABLE_PON);
    // Wait for power on
    ThisThread::sleep_for(3ms); 
    // Enable the ADC
    writeRegister(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);

    // Set integration time 
    //Integration Time = 2.4 ms × (256 − ATIME)
    // Max RGBC Count = (256 − ATIME) × 1024
    // 0xFF	2.4 ms (Default).       MAX COUNT 1024
    // 0xF6	24  ms                  MAX COUNT 10240
    // 0xD5	101 ms                  MAX COUNT 43008
    // 0xC0	154 ms                  MAX COUNT 65535
    // 0x00	700 ms                  MAX COUNT 65535
    writeRegister(TCS34725_ATIME, 0xF6);

    // Set gain 
    // 1x Gain:  0x00 (Default)      
    // 4x Gain:  0x01
    // 16x Gain: 0x02               
    // 60x Gain: 0x03
    writeRegister(TCS34725_AGAIN, 0x02);
}

// Write a value to a specified register
void TCS34725::writeRegister(uint8_t reg, uint8_t value) {
    char data[2] = { (char)(TCS34725_COMMAND_BIT | reg), (char)value };
    i2c->write(TCS34725_ADDRESS, data, 2);
}

// Read a 16-bit value from a specified register
// A two-byte read I2C transaction should be used with a read word protocol bit set in the command register. 
// When the lower byte register is read, the upper eight bits are stored into a shadow register, which is read by a subsequent read to the upper byte.
uint16_t TCS34725::read16(uint8_t reg) {
    char cmd = TCS34725_COMMAND_BIT | reg;
    char data[2] = {0};

    // Send the register address to read from
    i2c->write(TCS34725_ADDRESS, &cmd, 1);
    i2c->read(TCS34725_ADDRESS, data, 2);

    // Combine the two bytes into a 16-bit value (read word protocol)
    return (data[1] << 8) | data[0];
}

// Read color data (clear, red, green, blue)
void TCS34725::readColorData(uint16_t &clear, uint16_t &red, uint16_t &green, uint16_t &blue) {
    clear = read16(TCS34725_CDATAL);
    red = read16(TCS34725_RDATAL);
    green = read16(TCS34725_GDATAL);
    blue = read16(TCS34725_BDATAL);
}