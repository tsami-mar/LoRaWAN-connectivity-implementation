#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

#include "mbed.h"

class MoistureSensor {
public:
    // Constructor that initializes the AnalogIn pin for the sensor
    MoistureSensor(PinName pin);

    // Method to read and return the moisture level as a percentage
    float getMoisture();

    uint16_t getMoisture16();

private:
    AnalogIn sensor;  // Analog input object for the moisture sensor
};

#endif // MOISTURE_SENSOR_H
