#ifndef FIRMWARE_UTIL
#define FIRMWARE_UTIL

#include "mbed.h"
#include "igvc.pb.h"

/**
 * For defining constants and miscellaneous functions used by main.cpp
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

/* desired motor speed (as specified by the client) */
float g_desired_speed_l = 0;
float g_desired_speed_r = 0;

/* actual motor speeds */
float g_actual_speed_l = 0;
float g_actual_speed_r = 0;

/* PID constants */
float g_p_l = 0;
float g_d_l = 0;
float g_p_r = 0;
float g_d_r = 0;
float g_i_l = 0;
float g_i_r = 0;
float g_kv_l = 0;
float g_kv_r = 0;

/* motor outputs */
uint32_t g_left_output;
uint32_t g_right_output;

/* encoder values */
volatile int g_tick_data_right = 0;
volatile int g_tick_data_left = 0;

/* e-stop logic */
int g_estop = 1;

#endif //FIRMWARE_UTIL
