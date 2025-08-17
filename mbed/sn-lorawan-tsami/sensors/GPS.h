#ifndef FGPMMOPA6H_H
#define FGPMMOPA6H_H

#include "mbed.h"

class FGPMMOPA6H {
public:
    // Constructor to initialize GPS communication and enable pin
    FGPMMOPA6H(PinName tx, PinName rx, int baud_rate = 9600);

    // Method to read GPS data from the serial buffer
    void readData(int &gps_fix, int &num_satellites, float &latitude, float &longitude, char &meridian, 
                    char &parallel, float &altitude, char &measurement, char gps_time[10]);

    // A method to return the formatted GPS data string (for easy access)
    const char* getFormattedGPSData() const { return gps_data; }

private:
    // Method to convert NMEA coordinates to decimal degrees
    float convertToDecimalDegrees(float nmeaCoord);

    // Method to parse the NMEA sentence and extract relevant GPS information
    void parseData(const char* nmea_sentence, int &gps_fix, int &num_satellites, float &latitude, 
                           float &longitude, char &meridian, char &parallel, 
                           float &altitude, char &measurement, char gps_time[10]);
                           
    BufferedSerial gps_serial;  // Serial communication for GPS
    int gps_fix;
    int num_satellites;         // Number of satellites
    float latitude;             // Latitude
    float longitude;            // Longitude
    char meridian;              // Longitude direction (E/W)
    char parallel;              // Latitude direction (N/S)
    float altitude;             // Altitude
    char measurement;           // Measurement unit (e.g., meters)
    char gps_time[10];          // GPS time in HH:MM:SS format
    char gps_data[256];         // Formatted GPS data for later use
    char buffer[256];           // Buffer to read GPS data
    int timezone_offset = 1; 
};

#endif // FGPMMOPA6H_H
