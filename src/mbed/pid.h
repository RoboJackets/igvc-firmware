#ifndef PID_H
#define PID_H

#include "mbed.h"
#include "sabertooth_controller/sabertooth_controller.h"
#include "encoder_pair/encoder_pair.h"

void pid_thread(SaberToothController *motor_controller,
                float *d_t_sec,
                float *actual_speed_l, float *actual_speed_r,
                float *desired_speed_l, float *desired_speed_r,
                float *i_error_l, float *i_error_r,
                float *p_l, float *p_r,
                float *d_l, float *d_r,
                float *i_l, float *i_r,
                float *kv_l, float *kv_r,
                uint32_t *left_out, uint32_t *right_out);

Mutex pid_inputs;
Mutex pid_outputs;

#endif //PID_H
