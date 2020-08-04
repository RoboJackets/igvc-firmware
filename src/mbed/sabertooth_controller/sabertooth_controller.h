#ifndef ST_CONTROLLER
#define ST_CONTROLLER

#include "mbed.h"

// Sabertooth 2x60
// Motor 1: Right
// Motor 2: Left
class SabertoothController
{
  public:
    SabertoothController();
    explicit SabertoothController(PinName tx_pin);
    void stopMotors();
    uint32_t getLeftOutput();
    uint32_t getRightOutput();
    void setLeftMotor(int speed);
    void setRightMotor(int speed);
    void setSpeeds(int right_speed, int left_speed);

  private:
    RawSerial sabertooth;
    unsigned char left_output;
    unsigned char right_output;
};

#endif
