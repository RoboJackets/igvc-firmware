#ifndef FIRMWARE_UTIL
#define FIRMWARE_UTIL

#include "mbed.h"
#include "igvc.pb.h"

/**
 * For defining constants used by main.cpp
 * Do NOT use macros; instead define a constant here
 */

const bool g_debug = false;

/* ethernet setup variables */
const int g_server_port = 5333;
const int g_buffer_size = 256;
const int g_max_messages = 1; // backlog of messages the server should hold
const int g_timeout_ms = 50;  // timeout for blocking read operations
constexpr const char* g_mbed_ip = "192.168.1.20";
constexpr const char* g_netmask = "255.255.255.0";
constexpr const char* g_computer_ip = "192.168.1.21";


/* calculation constants */
const double g_wheel_circum = 1.092;
const double g_gear_ratio = 32.0;
const int g_ticks_per_rev = 48;
const double g_meters_per_tick = g_wheel_circum / (g_ticks_per_rev * g_gear_ratio);

#endif //FIRMWARE_UTIL
