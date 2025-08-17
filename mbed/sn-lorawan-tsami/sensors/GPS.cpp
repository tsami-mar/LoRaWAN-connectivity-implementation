#include "GPS.h"
#include <cstring>
#include <cstdlib>

// Constructor to initialize GPS communication and enable pin
FGPMMOPA6H::FGPMMOPA6H(PinName tx, PinName rx, int baud_rate)
    : gps_serial(tx, rx, baud_rate)
    
{
    memset(buffer, 0, sizeof(buffer));
}

// Convert NMEA coordinates to decimal degrees
float FGPMMOPA6H::convertToDecimalDegrees(float nmeaCoord) {
    int degrees = static_cast<int>(nmeaCoord) / 100;          // Extract degrees
    float minutes = nmeaCoord - (degrees * 100);              // Extract minutes
    return degrees + (minutes / 60.0f);                       // Convert to decimal degrees
}


void FGPMMOPA6H::readData(int &gps_fix, int &num_satellites, float &latitude, float &longitude, char &meridian, 
                          char &parallel, float &altitude, char &measurement, char gps_time[10]) {

    // Clear buffer before reading
    memset(buffer, 0, sizeof(buffer));

    if (gps_serial.readable()) {
        int bytesRead = gps_serial.read(buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Null-terminate the buffer
            
            // Check for a valid GPGGA sentence start
            char* start = strstr(buffer, "$GPGGA");
            if (start != nullptr) {
                parseData(start, gps_fix, num_satellites, latitude, longitude, meridian, 
                          parallel, altitude, measurement, gps_time);

            } 
        } 
    } 
    
}

// Parse the NMEA sentence to extract GPS information, updating the passed references
void FGPMMOPA6H::parseData(const char* nmea_sentence, int &gps_fix, int &num_satellites, float &latitude, 
                           float &longitude, char &meridian, char &parallel, 
                           float &altitude, char &measurement, char gps_time[10]) {
    if (nmea_sentence[0] == '$' && nmea_sentence[1] == 'G' && nmea_sentence[2] == 'P' && 
        nmea_sentence[3] == 'G' && nmea_sentence[4] == 'G' && nmea_sentence[5] == 'A') {
        
        int fieldIndex = 0;
        int i = 0;
        int j = 0;
        char field[16] = "";

        // Parse through each field in the NMEA sentence
        while (nmea_sentence[i] != '\0') {
            if (nmea_sentence[i] == ',' || nmea_sentence[i + 1] == '\0') {
                field[j] = '\0';
                
                switch (fieldIndex) {
                    case 1: 
                    { // GPS time in UTC (hhmmss)
                        int hour = atoi(field) / 10000;
                        int minute = (atoi(field) % 10000) / 100;
                        int second = atoi(field) % 100;

                        // Add timezone offset
                        hour += timezone_offset;
                        if (hour >= 24) { // Handle day rollover
                            hour -= 24;
                        }

                        // Format time into the provided buffer
                        snprintf(gps_time, 10, "%.2d:%.2d:%.2d", hour, minute, second);
                        break;
                    }
                    //  case 1: snprintf(gps_time, 10, "%.2s:%.2s:%.2s", field, field + 2, field + 4); break;
                    case 2: latitude = convertToDecimalDegrees(atof(field)); break;
                    case 3: parallel = field[0]; break;
                    case 4: longitude = convertToDecimalDegrees(atof(field)); break;
                    case 5: meridian = field[0]; break;
                    // case 6: fix quality 2-GPS 3-DGPS, at least 4 sats needed to get the altitude right
                    case 6: gps_fix = atoi(field); break;
                    case 7: num_satellites = atoi(field); break;
                    case 9: altitude = atof(field); break;
                    case 10: measurement = field[0] + 32; break;
                }
                
                j = 0;
                fieldIndex++;
            } else {
                field[j++] = nmea_sentence[i];
            }
            i++;
        }

        // Apply direction to latitude and longitude
        if (parallel == 'S') latitude = -latitude;
        if (meridian == 'W') longitude = -longitude;
   
    } 
}