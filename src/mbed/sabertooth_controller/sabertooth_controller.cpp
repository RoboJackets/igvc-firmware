#include "sabertooth_controller.h"
#include "mbed.h"

SaberToothController::SaberToothController()
        : sabertooth(p13, NC, 9600), left_output(0), right_output(0)
{
  stopMotors();
}

SaberToothController::SaberToothController(PinName tx_pin)
        : sabertooth(tx_pin, NC, 9600), left_output(0), right_output(0)
{
  stopMotors();
}

void SaberToothController::stopMotors()
{
  sabertooth.putc(0);
  left_output = 0;
  right_output = 0;
}

uint32_t SaberToothController::getLeftOutput()
{
  return static_cast<int>(left_output);
}

uint32_t SaberToothController::getRightOutput()
{
  return static_cast<int>(right_output);
}

void SaberToothController::setLeftMotor(int speed)
{
  // Motor 2: Left (128-255 for valid motor values)
  // motor outputs are inverted due to how the motors are mounted
  speed = min(63, max(-64, speed)) + 64;
  left_output = -static_cast<unsigned char>(speed + 128);
  sabertooth.putc(left_output);
}

void SaberToothController::setRightMotor(int speed)
{
  // Motor 1: Right (1-127 for valid motor values)
  // motor outputs are inverted due to how the motors are mounted
  speed = min(63, max(-63, speed)) + 64;
  right_output = -static_cast<unsigned char>(speed);
  sabertooth.putc(right_output);
}

void SaberToothController::setSpeeds(int right_speed, int left_speed)
{
  setLeftMotor(left_speed);
  setRightMotor(right_speed);
}
