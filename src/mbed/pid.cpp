#include "pid.h"

Mutex g_pid_control_lock;

void pid_thread(void const *args)
{
  auto *in_out = (ControlVars*) args;

  Timer timer;
  EncoderPair encoders;

  float d_t = 0;
  float i_err_l = 0, i_err_r = 0;
  MotorCoeffs coeffs{};
  MotorStatusPair motors{};
  int last_loop_time = 0;
  float error_l = 0, error_r = 0;
  float d_error_l = 0, d_error_r = 0;
  float actual_speed_last_l = 0, actual_speed_last_r = 0;
  float low_passed_pv_l = 0, low_passed_pv_r = 0;

  timer.reset();
  timer.start();

  // https://en.wikipedia.org/wiki/PID_controller#Discrete_implementation but with
  // e(t) on velocity, not position Changes to before 1: Derivative on PV 2:
  // Corrected integral 3: Low pass on Derivative 4: Clamping on Integral 5: Feed
  // forward
  while (true)
  {
    // Read input values
    g_pid_control_lock.lock();
    coeffs = *in_out->motor_coeffs;
    motors = *in_out->motor_pair;
    g_pid_control_lock.unlock();

    // 1: Calculate dt
    d_t = static_cast<float>(timer.read_ms() - last_loop_time) / 1000.0;

    if (timer.read() >= 1700)
    {
      timer.reset();
    }
    last_loop_time = timer.read_ms();

    // 2: Convert encoder values into velocity
    motors.left.actual_speed = (METERS_PER_TICK * encoders.getLeftTicks()) / d_t;
    motors.right.actual_speed = (METERS_PER_TICK * encoders.getRightTicks()) / d_t;

    // 3: Calculate error
    error_l = motors.left.desired_speed - motors.left.actual_speed;
    error_r = motors.right.desired_speed - motors.right.actual_speed;

    // 4: Calculate Derivative Error
    // TODO(oswinso): Make alpha a parameter
    float alpha = 0.75;
    low_passed_pv_l = alpha * (actual_speed_last_l - motors.left.actual_speed) / d_t + (1 - alpha) * low_passed_pv_l;
    low_passed_pv_r = alpha * (actual_speed_last_r - motors.right.actual_speed) / d_t + (1 - alpha) * low_passed_pv_r;

    d_error_l = low_passed_pv_l;
    d_error_r = low_passed_pv_r;

    // 5: Calculate Integral Error
    // 5a: Calculate Error
    i_err_l += error_l * d_t;
    i_err_r += error_r * d_t;

    // 5b: Perform clamping
    // TODO(oswinso): make clamping a parameter
    float i_clamp = 60 / coeffs.left.k_i;
    i_err_l = min(i_clamp, max(-i_clamp, i_err_l));
    i_err_r = min(i_clamp, max(-i_clamp, i_err_r));

    // 6: Sum P, I and D terms
    float feedback_left = coeffs.left.k_p * error_l + coeffs.left.k_d * d_error_l + coeffs.left.k_i * i_err_l;
    float feedback_right = coeffs.right.k_p * error_r + coeffs.right.k_d * d_error_r + coeffs.right.k_i * i_err_r;

    // 7: Calculate feedforward
    float feedforward_left = coeffs.left.k_kv * motors.left.desired_speed;
    float feedforward_right = coeffs.right.k_kv * motors.right.desired_speed;

    int left_signal = static_cast<int>(round(feedforward_left + feedback_left));
    int right_signal = static_cast<int>(round(feedforward_right + feedback_right));

    // 8: Deadband
    if (abs(motors.left.actual_speed) < 0.16 && abs(motors.left.desired_speed) < 0.16)
    {
      left_signal = 0;
    }
    if (abs(motors.right.actual_speed) < 0.16 && abs(motors.right.desired_speed) < 0.16)
    {
      right_signal = 0;
    }

    actual_speed_last_l = motors.left.actual_speed;
    actual_speed_last_r = motors.right.actual_speed;

    // Write output values
    g_pid_control_lock.lock();
    (*in_out->motor_controller).setSpeeds(right_signal, left_signal);
    motors.left.ctrl_output = (*in_out->motor_controller).getLeftOutput();
    motors.right.ctrl_output = (*in_out->motor_controller).getRightOutput();
    *in_out->d_t_sec = d_t;
    *in_out->i_error_l = i_err_l;
    *in_out->i_error_r = i_err_r;
    *in_out->motor_pair = motors;
    g_pid_control_lock.unlock();
  }
}