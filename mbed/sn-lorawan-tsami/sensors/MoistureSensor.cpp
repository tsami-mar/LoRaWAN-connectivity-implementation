#include "MoistureSensor.h"

// Constructor to initialize the sensor pin
MoistureSensor::MoistureSensor(PinName pin) : sensor(pin) {}

// Function to read the moisture level as a percentage
float MoistureSensor::getMoisture() {
    float analog_value = sensor.read(); // Read analog value (0.0 to 1.0)
    return analog_value * 100.0f;        // Convert to percentage (0% to 100%)
}

uint16_t MoistureSensor::getMoisture16() {
    float analog_value = sensor.read(); // Read analog value (0.0 to 1.0)
    uint16_t moisture = static_cast<uint16_t>(analog_value * 100.0f);  // Convert to percentage (0 to 100)
    return moisture;
}
