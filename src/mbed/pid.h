#ifndef PID_H
#define PID_H

#include "mbed.h"
#include "sabertooth_controller/sabertooth_controller.h"
#include "encoder_pair/encoder_pair.h"
#include "utils.h"

void pid_thread(void const *args);

struct PIDArgs
{
  SaberToothController *motor_controller;
  float *d_t_sec;
  float *i_error_l;
  float *i_error_r;
  MotorCoeffs *motor_coeffs;
  MotorStatusPair *motor_pair;
};

#endif //PID_H
