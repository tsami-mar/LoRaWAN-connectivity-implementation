/**
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdint>
#include <cstdio>

#include "mbed_version.h"
#include "mbed.h"

#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"
#include "events/EventQueue.h"

// Application helpers
#include "DummySensor.h"
#include "trace_helper.h"
#include "lora_radio_helper.h"

// Sensors
#include "sensors/Accelerometer.h"
#include "sensors/GPS.h"
#include "sensors/LightSensor.h"
#include "sensors/MoistureSensor.h"
#include "sensors/RGBsensor.h"
#include "sensors/TemperatureHumiditySensor.h"
#include "threads/gpsThread.h"

using namespace events;
using namespace std::chrono_literals;

// Max payload size can be LORAMAC_PHY_MAXPAYLOAD.
// This example only communicates with much shorter messages (<30 bytes).
// If longer messages are used, these buffers must be changed accordingly.
constexpr size_t TX_BUFFER_SIZE = 30;
uint8_t tx_buffer[TX_BUFFER_SIZE];
constexpr size_t RX_BUFFER_SIZE = 30;
uint8_t rx_buffer[RX_BUFFER_SIZE];

/*
 * Sets up an application dependent transmission timer in ms. Used only when Duty Cycling is off for testing
 */
#define TX_TIMER                        10s

/**
 * Maximum number of events for the event queue.
 * 10 is the safe number for the stack events, however, if application
 * also uses the queue for whatever purposes, this number should be increased.
 */
#define MAX_NUMBER_OF_EVENTS            10

/**
 * Maximum number of retries for CONFIRMED messages before giving up
 */
#define CONFIRMED_MSG_RETRY_COUNTER     3

/**
 * Dummy pin for dummy sensor
 */
#define PC_9                            0

/**
 * Dummy sensor class object
 */
DS1820  ds1820(PC_9);


/**
* This event queue is the global event queue for both the
* application and stack. To conserve memory, the stack is designed to run
* in the same thread as the application and the application is responsible for
* providing an event queue to the stack that will be used for ISR deferment as
* well as application information event queuing.
*/
static EventQueue ev_queue(MAX_NUMBER_OF_EVENTS *EVENTS_EVENT_SIZE);

/**
 * Event handler.
 *
 * This will be passed to the LoRaWAN stack to queue events for the
 * application which in turn drive the application.
 */
static void lora_event_handler(lorawan_event_t event);

/**
 * Constructing Mbed LoRaWANInterface and passing it the radio object from lora_radio_helper.
 */
static LoRaWANInterface lorawan(radio);

/**
 * Application specific callbacks
 */
static lorawan_app_callbacks_t callbacks;

/**
 * Default and configured device EUI, application EUI and application key
 */
static const uint8_t DEFAULT_DEV_EUI[] = {0x40, 0x39, 0x32, 0x35, 0x59, 0x37, 0x91, 0x94};
static uint8_t DEV_EUI[] = {0x8C, 0x39, 0x32, 0x35, 0x59, 0x37, 0x91, 0x94};
static uint8_t APP_EUI[] = {0x70, 0xb3, 0xd5, 0x7e, 0xd0, 0x00, 0xfc, 0x4d};
static uint8_t APP_KEY[] = {0xf3, 0x1c, 0x2e, 0x8b, 0xc6, 0x71, 0x28, 0x1d,
                            0x51, 0x16, 0xf0, 0x8f, 0xf0, 0xb7, 0x92, 0x8f};

/**
 * From previous project
 */

// For I2C communication
static I2C i2c(PB_9, PB_8);     //(SDA, SCL)

// RGB LED
DigitalOut redLED(PB_15), greenLED(PB_14), blueLED(PA_11);

// Sensors
MMA8451 accelerometer(&i2c);
Si7021 tempHumiditySensor(&i2c);
TCS34725 colorSensor(&i2c);
FGPMMOPA6H gps(PA_9, PA_10);
LightSensor lightSensor(PA_4);
MoistureSensor moistureSensor(PA_0);

// Define variables to store sensor data
uint16_t accelX = 0, accelY = 0, accelZ = 0;
uint16_t temperature = 0, humidity = 0;
uint16_t clear = 0, red = 0, green = 0, blue = 0;
uint16_t moisture = 0, brightness = 0;

int gps_fix = 0;
int num_satellites = 0;
float latitude = 0.0, longitude = 0.0, altitude = 0.0;
char meridian = ' ', parallel = ' ', measurement = ' ';
char gps_time[10] = {0};
Mutex gps_mutex;

// predefined coordinates set in Lisboa
#define DEFAULT_LATITUDE   38.7369;
#define DEFAULT_LONGITUDE  -9.1426;

static void prepare_message();
static void print_payload_to_serial(uint8_t* buffer, size_t length);

/**
 * Entry point for application
 */
int main(void)
{
    // Initialize and start the GPS thread
    gpsThread ThreadObj(&gps, &gps_fix, &num_satellites, &latitude, &longitude, &meridian, &parallel, &altitude, &measurement, gps_time);
    Thread thread(osPriorityNormal, 1024); 
    thread.start(callback(&ThreadObj, &gpsThread::run));

    redLED = 1; greenLED = 1; blueLED= 1;

    printf("\r\n*** Sensor Networks @ ETSIST, UPM ***\r\n"
           "   Mbed (v%d.%d.%d) LoRaWAN example\r\n",
           MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    printf("\r\n DEV_EUI: ");
    for (int i = 0; i < sizeof(DEV_EUI); ++i) printf("%02x", DEV_EUI[i]);
    printf("\r\n APP_EUI: ");
    for (int i = 0; i < sizeof(APP_EUI); ++i) printf("%02x", APP_EUI[i]);
    printf("\r\n APP_KEY: ");
    for (int i = 0; i < sizeof(APP_KEY); ++i) printf("%02x", APP_KEY[i]);
    printf("\r\n");

    if (!memcmp(DEV_EUI, DEFAULT_DEV_EUI, sizeof(DEV_EUI))) {
        printf("\r\n *** You are using the default device EUI value!!! *** \r\n");
        printf("Please, change it to ensure that the device EUI is unique \r\n");
        return -1;
    }

    // setup tracing
    setup_trace();

    // stores the status of a call to LoRaWAN protocol
    lorawan_status_t retcode;

    // Initialize LoRaWAN stack
    if (lorawan.initialize(&ev_queue) != LORAWAN_STATUS_OK) {
        printf("\r\n LoRa initialization failed! \r\n");
        return -1;
    }

    printf("\r\n Mbed LoRaWANStack initialized \r\n");

    // prepare application callbacks
    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);

    // Set number of retries in case of CONFIRMED messages
    if (lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER)
            != LORAWAN_STATUS_OK) {
        printf("\r\n set_confirmed_msg_retries failed! \r\n\r\n");
        return -1;
    }

    printf("\r\n CONFIRMED message retries : %d \r\n",
           CONFIRMED_MSG_RETRY_COUNTER);

    // Enable adaptive data rate
    if (lorawan.enable_adaptive_datarate() != LORAWAN_STATUS_OK) {
        printf("\r\n enable_adaptive_datarate failed! \r\n");
        return -1;
    }

    printf("\r\n Adaptive data  rate (ADR) - Enabled \r\n");

    lorawan_connect_t connect_params;
    connect_params.connect_type = LORAWAN_CONNECTION_OTAA;
    connect_params.connection_u.otaa.dev_eui = DEV_EUI;
    connect_params.connection_u.otaa.app_eui = APP_EUI;
    connect_params.connection_u.otaa.app_key = APP_KEY;
    connect_params.connection_u.otaa.nb_trials = 3;

    retcode = lorawan.connect(connect_params);

    if (retcode == LORAWAN_STATUS_OK ||
            retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS) {
    } else {
        printf("\r\n Connection error, code = %d \r\n", retcode);
        return -1;
    }

    printf("\r\n Connection - In Progress ...\r\n");

    // make your event queue dispatching events forever
    ev_queue.dispatch_forever();

    return 0;
}

/**
 * Sends a message to the Network Server
 */
static void send_message()
{
    uint16_t packet_len;
    int16_t retcode;
    int32_t sensor_value;

    prepare_message();
    /**
    * Debug print */
    printf("COLOR SENSOR: Clear: %d, Red: %d, Green: %d, Blue: %d \t \n", clear, red, green, blue); 
    printf("ACCELEROMETER raw: X_axis: %d, Y_axis: %d, Z_axis: %d\n", accelX , accelY, accelZ);
    printf("brightness: %d\n", brightness);
    printf("moisture: %d\n", moisture);
    printf("GPS: #Sats: %d Lat(UTC): %.6f %c Long(UTC): %.6f %c GPS FIX: %d %c GPS_time: %s\n", 
               num_satellites, latitude, parallel, longitude, meridian, gps_fix, measurement, gps_time);
    
 
    retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, tx_buffer, TX_BUFFER_SIZE,
                           MSG_UNCONFIRMED_FLAG);

    if (retcode < 0) {
        retcode == LORAWAN_STATUS_WOULD_BLOCK ? printf("send - WOULD BLOCK\r\n")
        : printf("\r\n send() - Error code %d \r\n", retcode);

        if (retcode == LORAWAN_STATUS_WOULD_BLOCK) {
            //retry in 3 seconds
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                ev_queue.call_in(3s, send_message);
            }
        }
        return;
    }

    printf("\r\n %d bytes scheduled for transmission \r\n", retcode);
    memset(tx_buffer, 0, sizeof(tx_buffer));
}

/**
 * Receive a message from the Network Server
 */
static void receive_message()
{
    uint8_t port;
    int flags;
    int16_t retcode = lorawan.receive(rx_buffer, sizeof(rx_buffer), port, flags);

    if (retcode < 0) {
        printf("\r\n receive() - Error code %d \r\n", retcode);
        return;
    }

    printf(" RX Data on port %u (%d bytes): ", port, retcode);
    for (uint8_t i = 0; i < retcode; i++) {
        printf("%02x ", rx_buffer[i]);
    }
    printf("\r\n");

    if (strcmp((char *)rx_buffer, "OFF") == 0) {
        redLED = 1; greenLED = 1; blueLED= 1;
        printf("LED turned OFF\r\n");
    }
    else if (strcmp((char *)rx_buffer, "Green") == 0) {
        redLED = 1; greenLED = 0; blueLED= 1;
        printf("Green LED ON\r\n");
    }
    else if (strcmp((char *)rx_buffer, "Red") == 0) {
        redLED = 0; greenLED = 1; blueLED = 1;
        printf("Red LED ON\r\n");
    }
    else {
        printf("Unknown command\r\n");
    }
    
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

/**
 * Event handler
 */
static void lora_event_handler(lorawan_event_t event)
{
    switch (event) {
        case CONNECTED:
            printf("\r\n Connection - Successful \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            } else {
                ev_queue.call_every(TX_TIMER, send_message);
            }

            break;
        case DISCONNECTED:
            ev_queue.break_dispatch();
            printf("\r\n Disconnected Successfully \r\n");
            break;
        case TX_DONE:
            printf("\r\n Message Sent to Network Server \r\n\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            }
            break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("\r\n Transmission Error - EventCode = %d \r\n", event);
            // try again
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            }
            break;
        case RX_DONE:
            printf("\r\n Received message from Network Server \r\n");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            printf("\r\n Error in reception - Code = %d \r\n", event);
            break;
        case JOIN_FAILURE:
            printf("\r\n OTAA Failed - Check Keys \r\n");
            break;
        case UPLINK_REQUIRED:
            printf("\r\n Uplink required by NS \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            }
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}

/**
 * Prepares the message with all the values
 */
static void prepare_message()
{
    colorSensor.readColorData(clear, red, green, blue);
    accelX = accelerometer.getXraw();
    accelY = accelerometer.getYraw();
    accelZ = accelerometer.getZraw();
    brightness= static_cast<uint16_t>(lightSensor.getBrightness() * 10); // Multiply by 10 to preserve one decimal point
    moisture= static_cast<uint16_t>(moistureSensor.getMoisture() * 10); // Multiply by 10 to preserve one decimal point
    temperature = tempHumiditySensor.readData(CMD_MEASURE_TEMP);
    humidity = tempHumiditySensor.readData(CMD_MEASURE_HUMIDITY);
   
    gps_mutex.lock();
    // Use default location if no GPS fix
    float currentLatitude = (gps_fix != 0) ? latitude : DEFAULT_LATITUDE;
    float currentLongitude = (gps_fix != 0) ? longitude : DEFAULT_LONGITUDE;
    gps_mutex.unlock();

    size_t pos = 0;
    tx_buffer[pos++] = clear & 0xff;
    tx_buffer[pos++] = (clear >> 8) & 0xff;

    tx_buffer[pos++] = red & 0xff;
    tx_buffer[pos++] = (red >> 8) & 0xff;

    tx_buffer[pos++] = green & 0xff;
    tx_buffer[pos++] = (green >> 8) & 0xff;

    tx_buffer[pos++] = blue & 0xff;
    tx_buffer[pos++] = (blue >> 8) & 0xff;

    tx_buffer[pos++] = accelX & 0xff;
    tx_buffer[pos++] = (accelX >> 8) & 0xff; 

    tx_buffer[pos++] = accelY & 0xff;
    tx_buffer[pos++] = (accelY >> 8) & 0xff;
    
    tx_buffer[pos++] = accelZ & 0xff;
    tx_buffer[pos++] = (accelZ >> 8) & 0xff;

    tx_buffer[pos++] = brightness & 0xff;
    tx_buffer[pos++] = (brightness >> 8) & 0xff;

    tx_buffer[pos++] = moisture & 0xff;
    tx_buffer[pos++] = (moisture >> 8) & 0xff;

    tx_buffer[pos++] = temperature & 0xff;
    tx_buffer[pos++] = (temperature >> 8) & 0xff;

    tx_buffer[pos++] = humidity & 0xff;
    tx_buffer[pos++] = (humidity >> 8) & 0xff;

    uint32_t latitude_as_u32 = *(uint32_t *)&currentLatitude; // Get the raw bytes of the float
    tx_buffer[pos++] = latitude_as_u32 & 0xff;
    tx_buffer[pos++] = (latitude_as_u32 >> 8) & 0xff;
    tx_buffer[pos++] = (latitude_as_u32 >> 16) & 0xff;
    tx_buffer[pos++] = (latitude_as_u32 >> 24) & 0xff;

    uint32_t longitude_as_u32 = *(uint32_t *)&currentLongitude; // Get the raw bytes of the float
    tx_buffer[pos++] = longitude_as_u32 & 0xff;
    tx_buffer[pos++] = (longitude_as_u32 >> 8) & 0xff;
    tx_buffer[pos++] = (longitude_as_u32 >> 16) & 0xff;
    tx_buffer[pos++] = (longitude_as_u32 >> 24) & 0xff;

    // Debug: Print tx_buffer via SerialPC
    print_payload_to_serial(tx_buffer, pos);
}


static void print_payload_to_serial(uint8_t* buffer, size_t length) {
    printf("Payload: ");
    for (size_t i = 0; i < length; i++) {
        printf("%02X", buffer[i]); // Print each byte as a 2-digit hex value
    }
    printf("\n"); // End the line after printing all bytes
}

// EOF
