#ifndef FIRMWARE_UTIL
#define FIRMWARE_UTIL

#include "mbed.h"
#include "igvc.pb.h"

/**
 * For defining constants used by main.cpp
 * Do NOT use macros; instead define a constant here
 */

constexpr bool DEBUG = false;

/* ethernet setup variables */
constexpr int SERVER_PORT = 5333;
constexpr int BUFFER_SIZE = 256;
constexpr const char* MBED_IP = "192.168.1.20";
constexpr const char* NETMASK = "255.255.255.0";
constexpr const char* COMPUTER_IP = "192.168.1.21";


/* calculation constants */
constexpr double WHEEL_CIRCUM = 1.092;
constexpr double GEAR_RATIO = 32.0;
constexpr int TICKS_PER_REV = 48;
constexpr double METERS_PER_TICK = WHEEL_CIRCUM / (TICKS_PER_REV * GEAR_RATIO);

#endif //FIRMWARE_UTIL
