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


/**
 * These structs are used to represent important data about the motors
 */
struct PIDCoeffs
{
  float k_p = 0.0f;
  float k_i = 0.0f;
  float k_d = 0.0f;
  float k_kv = 0.0f;
};

struct MotorCoeffs
{
  PIDCoeffs left_motor{};
  PIDCoeffs right_motor{};
};

struct MotorStatus
{
  float desired_speed = 0.0f;
  float actual_speed = 0.0f;
  uint32_t ctrl_output = 0;
};

struct MotorPair
{
  MotorStatus left_motor{};
  MotorStatus right_motor{};
};

#endif //FIRMWARE_UTIL
