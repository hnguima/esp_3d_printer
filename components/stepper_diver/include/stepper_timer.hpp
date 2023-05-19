#include "stepper_driver.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include <vector>

class StepperTimer
{
protected:
  static StepperTimer *_singleton;
  StepperTimer();

public:
  void add_stepper(Stepper *stepper);

  static StepperTimer *get_instance();

  StepperTimer(StepperTimer &other) = delete;
  void operator=(const StepperTimer &) = delete;

  const std::vector<Stepper *> &steppers() const { return _steppers; }

private:
  std::vector<Stepper *> _steppers;
};