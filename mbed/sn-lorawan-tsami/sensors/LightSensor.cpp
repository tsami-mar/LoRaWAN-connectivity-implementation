#include "LightSensor.h"

// Constructor to initialize the sensor pin
LightSensor::LightSensor(PinName pin) : sensor(pin) {}

// Function to read the brightness level as a percentage
float LightSensor::getBrightness() {
    float analog_value = sensor.read(); // Read analog value (0.0 to 1.0)
    return analog_value * 100.0f;        // Convert to percentage (0% to 100%)
}

uint16_t LightSensor::getBrightness16() {
    float analog_value = sensor.read(); // Read analog value (0.0 to 1.0)
    uint16_t brightness = static_cast<uint16_t>(analog_value * 100.0f);  // Convert to percentage (0 to 100)
    return brightness;
}
