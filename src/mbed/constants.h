#ifndef FIRMWARE_UTIL
#define FIRMWARE_UTIL

#include "mbed.h"
#include "igvc.pb.h"

/**
 * For defining constants used by main.cpp
 * Do NOT use macros; instead define a constant here
 */

constexpr bool G_DEBUG = false;

/* ethernet setup variables */
constexpr int G_SERVER_PORT = 5333;
constexpr int G_BUFFER_SIZE = 256;
constexpr const char* G_MBED_IP = "192.168.1.20";
constexpr const char* G_NETMASK = "255.255.255.0";
constexpr const char* G_COMPUTER_IP = "192.168.1.21";


/* calculation constants */
constexpr double G_WHEEL_CIRCUM = 1.092;
constexpr double G_GEAR_RATIO = 32.0;
constexpr int G_TICKS_PER_REV = 48;
constexpr double G_METERS_PER_TICK = G_WHEEL_CIRCUM / (G_TICKS_PER_REV * G_GEAR_RATIO);

#endif //FIRMWARE_UTIL
