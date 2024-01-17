#pragma once

#include <vector>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <esp_err.h>
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

#include "stepper_driver.hpp"
#include "gcode_interpreter.h"
#include "thermistor.h"
#include "pwm.h"
#include "pid.h"

#define PRINTER_COMMAND_QUEUE_SIZE 10

#define PRINTER_XY_CALIBRATION  81.07
#define PRINTER_XY_SPEED_CALIBRATION PRINTER_XY_CALIBRATION * 1.41421
#define PRINTER_Z_CALIBRATION -397.55
#define PRINTER_E_CALIBRATION 460
// #define PRINTER_E_CALIBRATION 417.2

void pcf_io_func(uint16_t pin, uint8_t value);
float therm_measure(void *param);
void pwm_control(void *param, float control);
void pwm_on_func(void *param);
void pwm_off_func(void *param);

typedef enum
{
  PRINT_STATUS_INITIALIZED,
  PRINT_STATUS_SETUP,
  PRINT_STATUS_PRINTING,
  PRINT_STATUS_DONE,
  PRINT_STATUS_ERROR,
} print_status_t;

class Printer
{
protected:
  static Printer *_singleton;
  Printer();

public:
  static void init();
  static Printer *get_instance();

  void move_all(double x, double y, double z, double e, double feedrate);
  void move_all_axis(double x, double y, double z, double feedrate);

  void move_x(double x, double speed);
  void move_x_in_time(double x, double time);
  void move_y(double y, double speed);
  void move_y_in_time(double y, double time);

  void move_xy(double x, double y, double speed);
  void move_xy_in_time(double x, double y, double time);

  void move_z(double z, double speed);
  void move_z_in_time(double z, double time);

  void move_e(double e, double speed);
  void move_e_in_time(double e, double time);

  void set_feedrate(int feedrate)
  {
    _feedrate = feedrate;
  }

  static void command_consumer();
  static void printer_task();
  static void print(std::string const &filename);
  static void print_temperatures();

  Stepper *stepper_m1;
  Stepper *stepper_m2;
  Stepper *stepper_z;
  Stepper *stepper_e;

private:
  uint32_t _feedrate = 0;
  double x_pos;
  double y_pos;
  double z_pos;
  double e_pos;

  float hotbed_temp;
  float hotbed_temp_setpoint;
  therm_t hotbed_sensor = THERMISTOR_DEFAULT_CONFIG;
  pwm_data_t hotbed_pwm{
      .period = 100,
      .duty_cycle = 0,
      .param = &hotbed_sensor,
      .on_func = pwm_on_func,
      .off_func = pwm_off_func,
  };
  pid_ctrl_t hotbed_pid = {
      .kp = 100,
      .ki = 1,
      .kd = 0,
      .tau = 1,
      .out_min = 0,
      .out_max = 100,
      .int_min = 0,
      .int_max = 100,
      .period = 0.5,
      .setpoint = 0,
      .sensor = &hotbed_sensor,
      .controller = &hotbed_pwm,
      .measure = therm_measure,
      .control = pwm_control,
  };

  float hotend_temp;
  float hotend_temp_setpoint;
  therm_t hotend_sensor = THERMISTOR_DEFAULT_CONFIG;
  pwm_data_t hotend_pwm{
      .period = 100,
      .duty_cycle = 0,
      .param = &hotend_sensor,
      .on_func = pwm_on_func,
      .off_func = pwm_off_func,
  };
  pid_ctrl_t hotend_pid = {
      .kp = 1.25,
      .ki = 0.06,
      .kd = 6.65,
      .tau = 1,
      .out_min = 0,
      .out_max = 80,
      .int_min = 0,
      .int_max = 50,
      .period = 0.5,
      .setpoint = 0,
      .sensor = &hotend_sensor,
      .controller = &hotend_pwm,
      .measure = therm_measure,
      .control = pwm_control,
  };

  std::queue<gcode_cmd_t> command_queue;

  std::mutex command_mutex;
  std::mutex command_queue_mutex;
  std::mutex print_mutex;

  std::condition_variable space_available;
  std::condition_variable command_available;
  std::condition_variable start_print;

  std::string filename;
  print_status_t print_status;

  static bool is_command_running(gcode_cmd_t command);
};