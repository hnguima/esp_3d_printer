#include "stepper_timer.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gptimer.h"

#include "esp_log.h"
#include "xtensa/core-macros.h"

static const char *TAG = "StepperTimer";

StepperTimer *StepperTimer::_singleton = nullptr;

static bool IRAM_ATTR stepper_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
  StepperTimer *stepper_timer = (StepperTimer *)user_data;

  // struct timeval tv;
  // gettimeofday(&tv, NULL);

  // ESP_DRAM_LOGI(TAG, "tick count: %lld", (int64_t)tv.tv_sec * 1000000L + (int64_t)tv.tv_usec);

  for (Stepper *stepper : stepper_timer->steppers())
  {
    // stepper->update();
  }

  return pdTRUE; // return whether we need to yield at the end of ISR
}

StepperTimer::StepperTimer()
{

  gptimer_handle_t gptimer = NULL;

  gptimer_config_t timer_config = {};
  timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;
  timer_config.direction = GPTIMER_COUNT_UP;
  timer_config.resolution_hz = 1000000; // 1MHz, 1 tick=1us
  ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

  // Rrgister Callbacks
  gptimer_event_callbacks_t callbacks = {};
  callbacks.on_alarm = stepper_timer_callback;
  ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &callbacks, this));

  ESP_ERROR_CHECK(gptimer_enable(gptimer));

  ESP_LOGI(TAG, "Start timer, auto-reload at alarm event");
  gptimer_alarm_config_t alarm_config = {};
  alarm_config.reload_count = 0;
  alarm_config.alarm_count = 100000;
  alarm_config.flags.auto_reload_on_alarm = true;

  ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
  ESP_ERROR_CHECK(gptimer_start(gptimer));
}

StepperTimer *StepperTimer::get_instance()
{
  ESP_LOGI(TAG, "deu certo a primeira chamada");

  if (_singleton == nullptr)
  {
    _singleton = new StepperTimer();
  }
  return _singleton;
}

void StepperTimer::add_stepper(Stepper *new_stepper)
{
  this->_steppers.push_back(new_stepper);
}