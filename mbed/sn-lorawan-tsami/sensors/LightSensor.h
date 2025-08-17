#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "mbed.h"

class LightSensor {
public:
    // Constructor that initializes the AnalogIn pin for the sensor
    LightSensor(PinName pin);

    // Method to read and return the brightness level as a percentage
    float getBrightness();

    uint16_t getBrightness16();

private:
    AnalogIn sensor;  // Analog input object for the brightness sensor
};

#endif // LIGHT_SENSOR_H
