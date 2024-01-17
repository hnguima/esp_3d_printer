#include "stepper_driver.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Drivers
#include "driver/gpio.h"

#include "esp_log.h"

#include <math.h>

static const char *TAG = "Stepper";

void default_io_func(uint16_t pin, uint8_t value)
{
  esp_err_t err = gpio_set_level((gpio_num_t)pin, value);

  if (err != ESP_OK)
  {
    ESP_DRAM_LOGI(TAG, "%d", err);
  }
}

static bool IRAM_ATTR stepper_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
  Stepper *stepper = (Stepper *)user_data;
  stepper->update();

  return pdTRUE; // return whether we need to yield at the end of ISR
}

Stepper::Stepper(uint16_t dir_io, uint16_t step_io, uint16_t en_io) : dir_io(dir_io),
                                                                      step_io(step_io),
                                                                      en_io(en_io)
{
  // init timer if not init
  // register itself to timer check array

  // load zero data and set
  set_dir_io_func(default_io_func);
  set_step_io_func(default_io_func);
  set_en_io_func(default_io_func);

  uint32_t frequency = 2000000;

  uint32_t microsteps = 16;
  steps_per_rev = 200 * microsteps;
  alpha = 2 * M_PI / (double)steps_per_rev; // 2 * PI / steps_per_rev

  a_t_100 = (uint32_t)(alpha * (double)frequency * 100); // alpha * timer_freq * 100
  t1_f_148 = (uint32_t)(0.00676 * (double)frequency);    // 0.00676 * timer_freq
  a_sq = (uint32_t)(alpha * 20000000000);                // alpha * 20000000000

  accel = 100000;
  step_c0 = (uint32_t)((double)t1_f_148 * sqrt((double)a_sq / (double)accel) / 100);

  gptimer_config_t timer_config = {};
  timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;
  timer_config.direction = GPTIMER_COUNT_UP;
  timer_config.resolution_hz = frequency; // 1MHz, 1 tick=1us
  ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer));

  // Rrgister Callbacks
  gptimer_event_callbacks_t callbacks = {};
  callbacks.on_alarm = (gptimer_alarm_cb_t)stepper_timer_callback;
  ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer, &callbacks, this));

  ESP_ERROR_CHECK(gptimer_enable(timer));
}
void Stepper::enable()
{
}

bool Stepper::setup_move(int32_t steps, double speed) // speed is step per second
{
  if (_is_moving == true)
  {
    ESP_LOGI(TAG, "Stepper is busy, try again later");
    return false;
  }

  dir_level = bool(steps >= 0);
  dir_io_func(dir_io, dir_level);

  speed = std::abs(speed);
  if (speed < 50)
  {
    speed = 50;
  }

  if (speed > 16000)
  {
    speed = 16000;
  }

  speed_target = (int32_t)(speed * 100 * alpha);
  step_count = 0;
  step_target = std::abs(steps);

  min_delay = a_t_100 / speed_target;

  max_s_lim = (uint32_t)((double)(speed_target * speed_target) / (2 * alpha * (double)accel * 100));

  accel_lim = (step_target) / 2;

  if (max_s_lim < accel_lim)
  {
    decel_steps = max_s_lim;
  }
  else if (max_s_lim >= accel_lim)
  {
    decel_steps = step_target - accel_lim;
  }

  decel_start = step_target - decel_steps;

  step_delay = step_c0;
  accel_count = 1;
  step_timer_cycles = 0;
  step_count = 0;

  // ESP_LOGI(TAG, "Start timer, auto-reload at alarm event");
  // printf("start delay %ld, min delay %ld\n", step_delay, min_delay);

  return true;
}

bool Stepper::start_move()
{
  if (_is_moving == true)
  {
    ESP_LOGI(TAG, "Stepper is busy, try again later");
    return false;
  }

  if (step_target == 0)
  {
    _is_moving = false;
    return false;
  }

  _is_moving = true;

  gptimer_alarm_config_t alarm_config = {};
  alarm_config.reload_count = 0;
  alarm_config.alarm_count = step_delay;
  alarm_config.flags.auto_reload_on_alarm = false;

  ESP_ERROR_CHECK(gptimer_set_alarm_action(timer, &alarm_config));
  gptimer_stop(timer);
  ESP_ERROR_CHECK(gptimer_start(timer));

  step_on = false;
  step_io_func(step_io, step_on);

  // _position = dir_level ? _position + 1 : _position - 1;

  return true;
}

void IRAM_ATTR Stepper::update()
{
  if (_is_moving == false)
  {
    return;
  }
  if (step_count >= step_target)
  {
    _is_moving = false;

    speed_target = 0;
    step_target = 0;

    step_delay = step_c0;

    accel_count = 0;
    step_count = 0;

    // step_timer_cycles = 0;

    return;
  }

  if (step_on == true)
  {
    // if step on just turn it off

    step_on = false;
    step_io_func(step_io, step_on);
  }
  else
  {
    step_on = true;
    step_io_func(step_io, step_on);

    _position = dir_level ? _position + 1 : _position - 1;

    if (step_delay > min_delay || accel_count <= 0)
    {
      // didnt reach target speed

      step_delay = step_delay - (2 * (int32_t)step_delay) / (4 * accel_count + 1);
    }
    else if (step_delay <= min_delay)
    {
      // reached target speed

      step_delay = min_delay;
      remainder = 0;
    }

    if (accel_count > 0 && step_count >= decel_start)
    {
      accel_count = -(int32_t)decel_steps;
    }

    accel_count++;
    step_count++;
  }

  gptimer_alarm_config_t alarm_config = {};
  alarm_config.reload_count = 0;
  alarm_config.alarm_count = step_delay / 2;
  alarm_config.flags.auto_reload_on_alarm = false;

  gptimer_set_raw_count(timer, 0);
  ESP_ERROR_CHECK(gptimer_set_alarm_action(timer, &alarm_config));
}