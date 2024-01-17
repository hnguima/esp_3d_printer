#pragma once

// FreeRTOS
#include "freertos/FreeRTOS.h"

#include <esp_err.h>

#include <esp_err.h>
#include <esp_log.h>

#include <string>
#include <functional>

#include "driver/gptimer.h"
#include "esp32/rom/ets_sys.h"

typedef struct
{
  uint32_t steps;
  uint8_t dir;
} step_dir_t;

using io_func = std::function<void((uint16_t pin, uint8_t value))>;

class Stepper
{

public:
  Stepper(uint16_t dir_io, uint16_t step_io, uint16_t en_io);

  void enable();
  void disable();

  void step(int32_t steps, int32_t speed);

  void set_zero();

  bool setup_move(int32_t steps, double speed);
  // bool setup_move(int32_t steps, double speed);
  // bool setup_move(step_dir_t move, double speed);
  bool start_move();

  void update();

  void set_dir_io_func(io_func const &func)
  {
    dir_io_func = func;
  };

  void set_step_io_func(io_func const &func)
  {
    step_io_func = func;
  };

  void set_en_io_func(io_func const &func)
  {
    en_io_func = func;
  };

  bool is_moving() const
  {
    return this->_is_moving;
  };

  int32_t position() const
  {
    return _position;
  };
  void set_position(int32_t position)
  {
    _position = position;
  };

private:
  uint16_t dir_io;
  uint8_t dir_level;
  io_func dir_io_func;

  uint16_t step_io;
  uint8_t step_level;
  io_func step_io_func;

  uint16_t en_io;
  uint8_t en_level;
  io_func en_io_func;

  int32_t _position = 0;

  uint32_t max_step = 5000;
  uint32_t max_speed = 200;

  uint32_t steps_per_rev;

  double alpha; // 2 * PI / steps_per_rev

  uint32_t a_t_100;  // alpha * timer_freq * 100
  uint32_t t1_f_148; // 0.00676 * timer_freq
  uint32_t a_sq;     // alpha * 20000000000

  uint32_t accel;
  uint32_t step_c0;

  // command variables - only set when motor is moving

  int32_t speed_target = 0;
  int32_t step_count = 0;
  int32_t step_target = 0;

  /*
    minimal delay needed to reach desired speed

    a_t_100 / target_speed
  */
  uint32_t min_delay; // a_t_100 / target_speed

  /*
  number of steps needed to achieve desired speed

  max_s_lim = (target_speed * target_speed) / (2 * alpha * accel * 100)
*/
  uint32_t max_s_lim;

  /*
    number of steps before decceleration needs to start

    accel_lim = (target_step * decel) / (accel + decel)
    if accel == decel: accel_lim = (target_step) / 2
  */
  uint32_t accel_lim;

  /*
    number of steps needed to decelerate

    if max_s_lim < accel_lim : decel_val = max_s_lim * accel / decel (if accel == decel, then is just max_s_lim)
    else if max_s_lim >= accel_lim : decel_val = target_steps - accel_lim
  */
  uint32_t decel_steps;

  /*
    step in which deceleration needs to start

    step_target - decel_steps
  */
  int32_t decel_start;

  // calculate in each step

  uint32_t timer_cycles = 0;
  uint32_t step_timer_cycles = 0;

  /*
    delay in between each step

    initial step delay: t1_f_148 * sqrt(a_sq / accel) / 100
    if step_delay > min_delay: new_step_delay = step_delay - (2 * step_delay + remainder) / (4 * accel_count + 1)
    else if step_delay <= min_delay: step_delay = min_delay
  */
  uint32_t step_delay;

  /*
    remainder for the step_delay calculation (could be ommited maybe?)

    initial remainder: 0
    if step_delay > min_delay: remainder = (2 * step_delay + remainder)(mod(4 * accel_count + 1))
    else if step_delay <= min_delay: 0
  */
  int32_t remainder;

  /*
    step count in acceleration or deceleration phases

    if step_delay > min_delay: starts at 0 and increases at each step
    else if step_count >= decel_start: starts at -decel_steps and ends at 0
    else : no change
  */
  int32_t accel_count;

  uint8_t step_on = 0;

  bool _is_moving = false;

  // SemaphoreHandle_t lock;

  int32_t cmd_step_count;

  gptimer_handle_t timer = nullptr;
};
