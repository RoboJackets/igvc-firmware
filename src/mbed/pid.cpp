#include "pid.h"

void pid_thread(SaberToothController *motor_controller, float *d_t_sec, float *actual_speed_l, float *actual_speed_r,
    float *desired_speed_l, float *desired_speed_r, float *i_error_l, float *i_error_r, float *p_l, float *p_r,
    float *d_l, float *d_r, float *i_l, float *i_r, float *kv_l, float *kv_r, uint32_t *left_out, uint32_t *right_out)
{
  Timer timer;
  EncoderPair encoders;

  float d_t;
  float act_speed_l, act_speed_r;
  float des_speed_l, des_speed_r;
  float i_err_l, i_err_r;
  float pp_l, pp_r;
  float dd_l, dd_r;
  float ii_l, ii_r;
  float kkvv_l, kkvv_r;
  int last_loop_time = 0;
  float error_l = 0, error_r = 0;
  float d_error_l = 0, d_error_r = 0;
  float actual_speed_last_l = 0, actual_speed_last_r = 0;
  float low_passed_pv_l = 0, low_passed_pv_r = 0;

  // Global Variables Used:
  // float g_d_t_sec --> *d_t_sec, d_t
  // float g_actual_speed_l/r --> *actual_speed_l/r, act_speed_l/r
  // float g_desired_speed_l/r --> *desired_speed_l/r, des_speed_l/r
  // float g_i_error_l/r --> *i_error_l/r, i_err_l/r
  // float g_p_l/r --> *p_l/r, pp_l/r
  // float g_d_l/r --> *d_l/r, dd_l/r
  // float g_i_l/r --> *i_l/r, ii_l/r
  // float g_kv_l/r --> *kv_l/r, kkvv_l/r
  // SaberToothController g_motor_controller --> *motor_controller
  // uint32_t g_left_output --> *left_output
  // uint32_t g_right_output --> *right_output

  timer.reset();
  timer.start();

  // https://en.wikipedia.org/wiki/PID_controller#Discrete_implementation but with
  // e(t) on velocity, not position Changes to before 1: Derivative on PV 2:
  // Corrected integral 3: Low pass on Derivative 4: Clamping on Integral 5: Feed
  // forward
  while (true)
  {
    // 1: Calculate dt
    g_d_t_sec = static_cast<float>(timer.read_ms() - last_loop_time) / 1000.0;

    if (timer.read() >= 1700)
    {
      timer.reset();
      last_loop_time = 0;
    }
    last_loop_time = timer.read_ms();

    // 2: Convert encoder values into velocity
    g_actual_speed_l = (G_METERS_PER_TICK * encoders.getLeftTicks()) / g_d_t_sec;
    g_actual_speed_r = (G_METERS_PER_TICK * encoders.getRightTicks()) / g_d_t_sec;

    // 3: Calculate error
    error_l = g_desired_speed_l - g_actual_speed_l;
    error_r = g_desired_speed_r - g_actual_speed_r;

    // 4: Calculate Derivative Error
    // TODO(oswinso): Make alpha a parameter
    float alpha = 0.75;
    low_passed_pv_l = alpha * (actual_speed_last_l - g_actual_speed_l) / g_d_t_sec + (1 - alpha) * low_passed_pv_l;
    low_passed_pv_r = alpha * (actual_speed_last_r - g_actual_speed_r) / g_d_t_sec + (1 - alpha) * low_passed_pv_r;

    d_error_l = low_passed_pv_l;
    d_error_r = low_passed_pv_r;

    // 5: Calculate Integral Error
    // 5a: Calculate Error
    g_i_error_l += error_l * g_d_t_sec;
    g_i_error_r += error_r * g_d_t_sec;

    // 5b: Perform clamping
    // TODO(oswinso): make clamping a parameter
    float i_clamp = 60 / g_i_l;
    g_i_error_l = min(i_clamp, max(-i_clamp, g_i_error_l));
    g_i_error_r = min(i_clamp, max(-i_clamp, g_i_error_r));

    // 6: Sum P, I and D terms
    float feedback_left = g_p_l * error_l + g_d_l * d_error_l + g_i_l * g_i_error_l;
    float feedback_right = g_p_r * error_r + g_d_r * d_error_r + g_i_r * g_i_error_r;

    // 7: Calculate feedforward
    float feedforward_left = g_kv_l * g_desired_speed_l;
    float feedforward_right = g_kv_r * g_desired_speed_r;

    int left_signal = static_cast<int>(round(feedforward_left + feedback_left));
    int right_signal = static_cast<int>(round(feedforward_right + feedback_right));

    // 8: Deadband
    if (abs(g_actual_speed_l) < 0.16 && abs(g_desired_speed_l) < 0.16)
    {
      left_signal = 0;
    }

    if (abs(g_actual_speed_r) < 0.16 && abs(g_desired_speed_r) < 0.16)
    {
      right_signal = 0;
    }

    g_motor_controller.setSpeeds(right_signal, left_signal);

    g_left_output = g_motor_controller.getLeftOutput();
    g_right_output = g_motor_controller.getRightOutput();

    actual_speed_last_l = g_actual_speed_l;
    actual_speed_last_r = g_actual_speed_r;
  }
}