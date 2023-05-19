#pragma once

// FreeRTOS
#include "freertos/FreeRTOS.h"

#include <esp_err.h>

#include <esp_err.h>
#include <esp_log.h>

#include <string>

class StepperTimer;

class Stepper
{

public:
  Stepper();

  void enable();
  void disable();

  void step(int32_t steps, int32_t speed);

  void set_zero();

  void update();

  int32_t max_step;
  int32_t max_speed;

  int32_t step_per_mm;
  int32_t step_per_rev;

private:
  int32_t step_count;
  int32_t speed;

  bool step_on;

  // SemaphoreHandle_t lock;

  int32_t cmd_step_count;

  StepperTimer *timer;
};
