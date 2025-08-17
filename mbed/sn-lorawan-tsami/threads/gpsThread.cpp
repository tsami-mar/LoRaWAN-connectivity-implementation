#include "gpsThread.h"

// Constructor: Initialize with sensors, GPS, and data pointers
gpsThread::gpsThread(FGPMMOPA6H* gps, int *gps_fix,
                           int *num_satellites, float *latitude, float *longitude, 
                           char *meridian, char *parallel, float *altitude, char *measurement, char gps_time[10])
    : gps(gps), gps_fix(gps_fix), num_satellites(num_satellites), latitude(latitude), longitude(longitude), 
      meridian(meridian), parallel(parallel), altitude(altitude), measurement(measurement), gps_time(gps_time) {}

extern Mutex gps_mutex;

// Function that runs in the thread, collecting data from sensors and GPS
void gpsThread::run() {
    while (true) {
        gps_mutex.lock();
        // Collect data from GPS
        gps->readData(*gps_fix, *num_satellites, *latitude, *longitude, *meridian, 
                        *parallel, *altitude, *measurement, gps_time);

        gps_mutex.unlock();

        ThisThread::sleep_for(2s);
    }
}

