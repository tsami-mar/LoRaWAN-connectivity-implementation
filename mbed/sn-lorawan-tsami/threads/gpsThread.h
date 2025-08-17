#ifndef GPSTHREAD_H
#define GPSTHREAD_H

#include "mbed.h"
#include "../sensors/GPS.h"  

class gpsThread {
public:
    // Constructor: Initialize with sensors, GPS, and data pointers
    gpsThread(FGPMMOPA6H* gps, int *gps_fix, int *num_satellites, float *latitude, float *longitude, 
                 char *meridian, char *parallel, float *altitude, char *measurement, char gps_time[10]);

    void run();     // Thread's main function to collect data


private:
    FGPMMOPA6H* gps;

    // Pointers to GPS data for storing results
    int* gps_fix;
    int* num_satellites;
    float* latitude;
    float* longitude;
    char* meridian;
    char* parallel;
    float* altitude;
    char* measurement;
    char* gps_time;

};

#endif // GPSTHREAD_H