#include "stepper_timer.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gptimer.h"
#include "esp32/rom/ets_sys.h"

#include "esp_log.h"
#include "xtensa/core-macros.h"

static const char *TAG = "StepperTimer";

static bool IRAM_ATTR stepper_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{

  // ets_printf("timer callback\n");

  // for (Stepper *stepper : ((StepperTimer *)user_data)->steppers())
  // {
  //   // stepper->update();
  // }

  return pdTRUE; // return whether we need to yield at the end of ISR
}

StepperTimer::StepperTimer()
{
  gptimer_handle_t gptimer = nullptr;

  gptimer_config_t timer_config = {};
  timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;
  timer_config.direction = GPTIMER_COUNT_UP;
  timer_config.resolution_hz = this->frequency(); // 1MHz, 1 tick=1us
  ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

  // Rrgister Callbacks
  gptimer_event_callbacks_t callbacks = {};
  callbacks.on_alarm = stepper_timer_callback;
  ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &callbacks, this));

  ESP_ERROR_CHECK(gptimer_enable(gptimer));

  ESP_LOGI(TAG, "Start timer, auto-reload at alarm event");
  gptimer_alarm_config_t alarm_config = {};
  alarm_config.reload_count = 0;
  alarm_config.alarm_count = 10;
  alarm_config.flags.auto_reload_on_alarm = true;

  ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
  ESP_ERROR_CHECK(gptimer_start(gptimer));
}

void StepperTimer::add_stepper(Stepper *new_stepper)
{
  this->_steppers.push_back(new_stepper);
}