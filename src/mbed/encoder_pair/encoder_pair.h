
#ifndef ENCODER_PAIR_H
#define ENCODER_PAIR_H

#include "mbed.h"

class EncoderPair
{
public:
  EncoderPair();
  explicit EncoderPair(bool double_ticks);
  int getLeftTicks();
  int getRightTicks();

private:
  InterruptIn left_encoder_a;
  DigitalIn left_encoder_b;
  InterruptIn right_encoder_a;
  DigitalIn right_encoder_b;
  volatile int left_tick_count;
  volatile int right_tick_count;
  void tickLeft();
  void tickRight();
};


#endif //ENCODER_PAIR_H
