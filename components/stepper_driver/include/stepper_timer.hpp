#pragma once
#include "stepper_driver.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include <vector>

class StepperTimer
{
protected:
public:
  StepperTimer();

  void add_stepper(Stepper *stepper);

  const uint32_t &frequency() const { return _frequency; };

  const std::vector<Stepper *> &steppers() const { return _steppers; };

private:
  std::vector<Stepper *> _steppers;

  uint32_t _frequency = 1 * 1000 * 1000; //1MHz
};