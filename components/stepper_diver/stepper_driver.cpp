#include "stepper_driver.hpp"
#include "stepper_timer.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "esp_log.h"

static const char *TAG = "Stepper";

Stepper::Stepper()
{
  // init timer if not init
  // register itself to timer check array

  // load zero data and set

  this->timer = StepperTimer::get_instance();
  timer->add_stepper(this);

}

void Stepper::update()
{
  // ESP_DRAM_LOGI(TAG, "stepper updating...");
}