
#include "encoder_pair.h"
#include "mbed.h"

EncoderPair::EncoderPair():
    left_encoder_a(p24),
    left_encoder_b(p23),
    right_encoder_a(p26),
    right_encoder_b(p25),
    left_tick_count(0),
    right_tick_count(0)
{
  left_encoder_a.rise(callback(this, &EncoderPair::tickLeft));
  right_encoder_a.rise(callback(this, &EncoderPair::tickRight));
}

EncoderPair::EncoderPair(bool double_ticks):
    left_encoder_a(p24),
    left_encoder_b(p23),
    right_encoder_a(p26),
    right_encoder_b(p25),
    left_tick_count(0),
    right_tick_count(0)
{
  left_encoder_a.rise(callback(this, &EncoderPair::tickLeft));
  right_encoder_a.rise(callback(this, &EncoderPair::tickRight));
  if (double_ticks)
  {
    left_encoder_a.fall(callback(this, &EncoderPair::tickLeft));
    right_encoder_a.fall(callback(this, &EncoderPair::tickRight));
  }
}

void EncoderPair::tickLeft()
{
  if (left_encoder_a.read() == left_encoder_b.read())
  {
    ++left_tick_count;
  }
  else
  {
    --left_tick_count;
  }
}

void EncoderPair::tickRight()
{
  if (right_encoder_a.read() == right_encoder_b.read())
  {
    ++right_tick_count;
  }
  else
  {
    --right_tick_count;
  }
}

int EncoderPair::getLeftTicks()
{
  int tick_count = left_tick_count;
  left_tick_count = 0;
  return tick_count;
}

int EncoderPair::getRightTicks()
{
  int tick_count = right_tick_count;
  right_tick_count = 0;
  return tick_count;
}
