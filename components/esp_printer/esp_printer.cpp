#include "esp_printer.hpp"

#include <iostream>
#include <iomanip>

#include <freertos/FreeRTOS.h>

#include <esp_log.h>
#include "driver/gpio.h"
#include "i2c.h"
#include "pcf8574.h"

#include "gcode_interpreter.h"

static const char *TAG = "Printer";

Printer *Printer::_singleton = nullptr;

#define PIN_PORT_PCF_0 (0x20 << 8)
#define PIN_PORT_PCF_1 (0x21 << 8)
#define PIN_PORT_PCF_2 (0x22 << 8)

#include <numeric>

static int32_t get_m1_steps(double x_displacement, double y_displacement)
{
  double m1_displacement = (x_displacement + y_displacement) * PRINTER_XY_CALIBRATION;

  return (int32_t)std::round(m1_displacement);
}

static int32_t get_m2_steps(double x_displacement, double y_displacement)
{
  double m2_displacement = ((x_displacement - y_displacement)) * PRINTER_XY_CALIBRATION;

  return (int32_t)std::round(m2_displacement);
}

static int32_t get_z_steps(double z_displacement)
{
  return (int32_t)std::round(z_displacement * PRINTER_Z_CALIBRATION);
}

static int32_t get_e_steps(double e_displacement)
{
  return (int32_t)std::round(e_displacement * PRINTER_E_CALIBRATION);
}

static double get_x_mm(int32_t m1_steps, int32_t m2_steps)
{
  return (double)(m1_steps + m2_steps) / (2 * PRINTER_XY_CALIBRATION);
}

static double get_y_mm(int32_t m1_steps, int32_t m2_steps)
{
  return (double)(m1_steps - m2_steps) / (2 * PRINTER_XY_CALIBRATION);
}

static double get_z_mm(int32_t z_steps)
{
  return (double)z_steps / PRINTER_Z_CALIBRATION;
}

static double get_e_mm(int32_t e_steps)
{
  return (double)e_steps / PRINTER_E_CALIBRATION;
}

void pcf_io_func(uint16_t pin, uint8_t value)
{
  pcf_set((uint8_t)(pin >> 8), (uint8_t)(pin & 0xF), value);
}

float therm_measure(void *param)
{
  therm_t *therm_data = (therm_t *)param;

  return therm_get_temp(therm_data);
}

void pwm_control(void *param, float control)
{
  pwm_data_t *pwm_data = (pwm_data_t *)param;

  pwm_data->duty_cycle = round(control);
}

void pwm_on_func(void *param)
{
  therm_t *therm_data = (therm_t *)param;

  if (therm_data->adc_ch == 1)
  {
    pcf_set(0x20, 4, 1);
  }
  else if (therm_data->adc_ch == 2)
  {
    pcf_set(0x20, 6, 1);
  }
}

void pwm_off_func(void *param)
{
  therm_t *therm_data = (therm_t *)param;

  if (therm_data->adc_ch == 1)
  {
    pcf_set(0x20, 4, 0);
  }
  else if (therm_data->adc_ch == 2)
  {
    pcf_set(0x20, 6, 0);
  }
}

Printer::Printer()
{
  adc_init();

  // init i2c
  i2c_init(I2C_NUM_0);

  // init pcf
  pcf_init(I2C_NUM_0);

  // init GPIO
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = (1 << 18) | (1 << 21) | (1 << 19) | (1 << 2) | (1 << 4) | (1 << 22) | (1 << 23) | (1 << 12);
  gpio_config(&io_conf);

  stepper_m2 = new Stepper(21, 19, PIN_PORT_PCF_2 | 2);
  stepper_m2->set_en_io_func(pcf_io_func);

  stepper_m1 = new Stepper(2, 4, PIN_PORT_PCF_2 | 1);
  stepper_m1->set_en_io_func(pcf_io_func);

  stepper_z = new Stepper(23, 22, PIN_PORT_PCF_2 | 3);
  stepper_z->set_en_io_func(pcf_io_func);

  stepper_e = new Stepper(12, 18, PIN_PORT_PCF_2 | 0);
  stepper_e->set_en_io_func(pcf_io_func);

  hotbed_sensor.adc_ch = 1;
  hotend_sensor.adc_ch = 2;

  therm_init(&hotbed_sensor);
  therm_init(&hotend_sensor);

  pwm_init(&hotbed_pwm);
  pwm_init(&hotend_pwm);

  pid_init(&hotbed_pid);
  pid_init(&hotend_pid);
}

Printer *Printer::get_instance()
{

  if (_singleton == nullptr)
  {
    _singleton = new Printer();
  }
  return _singleton;
}

void Printer::init()
{
  Printer const *printer = Printer::get_instance();

  _singleton->print_status = PRINT_STATUS_INITIALIZED;

  std::thread producer(Printer::printer_task);
  producer.detach();
  std::thread consumer(Printer::command_consumer);
  consumer.detach();

  // double x_displacement = 50;
  // double y_displacement = 0;
  // double z_displacement = 0;

  // // while (true)
  // // {

  // //   // // x_displacement = 50;
  // z_displacement = 50;
  // // y_displacement = 0;

  // _singleton->stepper_e->setup_move(get_e_step_dir(z_displacement), 500);
  // _singleton->stepper_e->start_move();

  // while (_singleton->stepper_e->is_moving())
  // {
  //   vTaskDelay(100 / portTICK_PERIOD_MS);
  // }

  //   // z_displacement = -50;
  //   // // y_displacement = 0;

  //   // _singleton->stepper_z->setup_move(get_z_step_dir(z_displacement), 500);
  //   // _singleton->stepper_z->start_move();

  //   // while (_singleton->stepper_z->is_moving())
  //   // {
  //   //   vTaskDelay(100 / portTICK_PERIOD_MS);
  //   // }

  //   // x_displacement = 0;
  //   // y_displacement = 50;

  //   // _singleton->stepper_m1->setup_move(get_m1_steps(x_displacement, y_displacement), 500);
  //   // _singleton->stepper_m2->setup_move(get_m2_steps(x_displacement, y_displacement), 500);
  //   // _singleton->stepper_m1->start_move();
  //   // _singleton->stepper_m2->start_move();

  //   // while (_singleton->stepper_m1->is_moving() || _singleton->stepper_m2->is_moving())
  //   // {
  //   //   vTaskDelay(100 / portTICK_PERIOD_MS);
  //   // }

  //   // x_displacement = -50;
  //   // y_displacement = 0;

  //   // _singleton->stepper_m1->setup_move(get_m1_steps(x_displacement, y_displacement), 500);
  //   // _singleton->stepper_m2->setup_move(get_m2_steps(x_displacement, y_displacement), 500);
  //   // _singleton->stepper_m1->start_move();
  //   // _singleton->stepper_m2->start_move();

  //   // while (_singleton->stepper_m1->is_moving() || _singleton->stepper_m2->is_moving())
  //   // {
  //   //   vTaskDelay(100 / portTICK_PERIOD_MS);
  //   // }

  //   // x_displacement = 0;
  //   // y_displacement = -50;

  //   // _singleton->stepper_m1->setup_move(get_m1_steps(x_displacement, y_displacement), 500);
  //   // _singleton->stepper_m2->setup_move(get_m2_steps(x_displacement, y_displacement), 500);
  //   // _singleton->stepper_m1->start_move();
  //   // _singleton->stepper_m2->start_move();

  //   // while (_singleton->stepper_m1->is_moving() || _singleton->stepper_m2->is_moving())
  //   // {
  //   //   vTaskDelay(100 / portTICK_PERIOD_MS);
  //   // }
  // }
}

void Printer::move_all_axis(double x, double y, double z, double feedrate)
{
}

void Printer::move_x(double x, double speed)
{

  int32_t m1_steps = get_m1_steps(x, 0);
  int32_t m2_steps = get_m2_steps(x, 0);

  _singleton->stepper_m1->setup_move(m1_steps, speed * PRINTER_XY_CALIBRATION);
  _singleton->stepper_m2->setup_move(m2_steps, speed * PRINTER_XY_CALIBRATION);

  _singleton->stepper_m1->start_move();
  _singleton->stepper_m2->start_move();
}

void Printer::move_x_in_time(double x, double time)
{

  int32_t m1_steps = get_m1_steps(x, 0);
  int32_t m2_steps = get_m2_steps(x, 0);

  _singleton->stepper_m1->setup_move(m1_steps, m1_steps / (time + 0.000000001));
  _singleton->stepper_m2->setup_move(m2_steps, m2_steps / (time + 0.000000001));

  _singleton->stepper_m1->start_move();
  _singleton->stepper_m2->start_move();
}

void Printer::move_y(double y, double speed)
{
  int32_t m1_steps = get_m1_steps(0, y);
  int32_t m2_steps = get_m2_steps(0, y);

  _singleton->stepper_m1->setup_move(m1_steps, speed * PRINTER_XY_CALIBRATION);
  _singleton->stepper_m2->setup_move(m2_steps, speed * PRINTER_XY_CALIBRATION);

  _singleton->stepper_m1->start_move();
  _singleton->stepper_m2->start_move();
}

void Printer::move_y_in_time(double y, double time)
{

  int32_t m1_steps = get_m1_steps(0, y);
  int32_t m2_steps = get_m2_steps(0, y);

  _singleton->stepper_m1->setup_move(m1_steps, m1_steps / (time + 0.000000001));
  _singleton->stepper_m2->setup_move(m2_steps, m2_steps / (time + 0.000000001));

  _singleton->stepper_m1->start_move();
  _singleton->stepper_m2->start_move();
}

#ifndef M_PI_4
#define M_PI_4 (3.1415926535897932384626433832795 / 4.0)
#endif

void Printer::move_xy(double x, double y, double speed)
{

  int32_t m1_steps = get_m1_steps(x, y);
  int32_t m2_steps = get_m2_steps(x, y);

  double angle = atan(y / (x + 0.000001)) + M_PI_4;
  speed = speed * PRINTER_XY_SPEED_CALIBRATION;
  double step_speed_m1 = speed * sin(angle);
  double step_speed_m2 = speed * cos(angle);

  _singleton->stepper_m1->setup_move(m1_steps, step_speed_m1);
  _singleton->stepper_m2->setup_move(m2_steps, step_speed_m2);

  _singleton->stepper_m1->start_move();
  _singleton->stepper_m2->start_move();
}

void Printer::move_xy_in_time(double x, double y, double time)
{

  int32_t m1_steps = get_m1_steps(x, y);
  int32_t m2_steps = get_m2_steps(x, y);

  _singleton->stepper_m1->setup_move(m1_steps, m1_steps / (time + 0.000000001));
  _singleton->stepper_m2->setup_move(m2_steps, m2_steps / (time + 0.000000001));

  _singleton->stepper_m1->start_move();
  _singleton->stepper_m2->start_move();
}

void Printer::move_z(double z, double speed)
{
}

void Printer::move_z_in_time(double z, double time)
{

  int32_t z_steps = get_z_steps(z);

  _singleton->stepper_z->setup_move(z_steps, (time != 0) ? (double)(z_steps / time) : (_singleton->_feedrate * PRINTER_Z_CALIBRATION));
  _singleton->stepper_z->start_move();
}

void Printer::move_e(double e, double speed)
{
}

void Printer::move_e_in_time(double e, double time)
{

  int32_t e_steps = get_e_steps(e);

  // double speed = (time != 0) ? (double)(e_steps / time) : (_singleton->_feedrate * PRINTER_E_CALIBRATION);
  // printf("speed: %lf\n", speed);

  _singleton->stepper_e->setup_move(e_steps, (time != 0) ? (double)(e_steps / time) : (_singleton->_feedrate * PRINTER_E_CALIBRATION));
  _singleton->stepper_e->start_move();
}

void Printer::print_temperatures()
{
  ESP_LOGI(TAG, "hotbed temp: %f, setpoint: %f", _singleton->hotbed_pid.measure(_singleton->hotbed_pid.sensor), _singleton->hotbed_pid.setpoint);
  ESP_LOGI(TAG, "hotend temp: %f, setpoint: %f", _singleton->hotend_pid.measure(_singleton->hotend_pid.sensor), _singleton->hotend_pid.setpoint);
}

bool Printer::is_command_running(gcode_cmd_t command)
{
  if ((command.has_x || command.has_y) &&
      (_singleton->stepper_m1->is_moving() || _singleton->stepper_m2->is_moving()))
  {
    // printf("waiting on x or y\n");
    return true;
  }
  if (command.has_z && _singleton->stepper_z->is_moving())
  {
    // printf("waiting on z\n");
    return true;
  }
  if (command.has_ext && _singleton->stepper_e->is_moving())
  {
    // printf("waiting on e\n");
    return true;
  }
  if (command.has_hotbed_temp &&
      _singleton->hotbed_pid.setpoint > _singleton->hotbed_pid.measure(_singleton->hotbed_pid.sensor))

  {
    _singleton->print_temperatures();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }
  if (command.has_hotend_temp &&
      _singleton->hotend_pid.setpoint > _singleton->hotend_pid.measure(_singleton->hotend_pid.sensor))
  {
    _singleton->print_temperatures();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }

  return false;
}

double m1_max = 0, m1_min = 0, m2_max = 0, m2_min = 0;

void Printer::command_consumer()
{

  while (true)
  {
    gcode_cmd_t command;

    {
      std::unique_lock command_queue_lock(_singleton->command_queue_mutex);
      _singleton->command_available.wait(command_queue_lock, []
                                         { return _singleton->command_queue.empty() != true; });

      command = _singleton->command_queue.front();
      _singleton->command_queue.pop();
      _singleton->space_available.notify_one();
    }

    _singleton->x_pos = get_x_mm(_singleton->stepper_m1->position(), _singleton->stepper_m2->position());
    _singleton->y_pos = get_y_mm(_singleton->stepper_m1->position(), _singleton->stepper_m2->position());
    _singleton->z_pos = get_z_mm(_singleton->stepper_z->position());
    _singleton->e_pos = get_e_mm(_singleton->stepper_e->position());

    double x_displacement = command.has_x ? command.x - _singleton->x_pos : 0;
    double y_displacement = command.has_y ? command.y - _singleton->y_pos : 0;
    double z_displacement = command.has_z ? command.z - _singleton->z_pos : 0;
    double e_displacement = command.has_ext ? command.ext - _singleton->e_pos : 0;

    if (command.has_feed_rate && command.feed_rate > 0)
    {
      _singleton->set_feedrate(command.feed_rate / 60);
    }

    double time = sqrt(std::pow(x_displacement, 2) + std::pow(y_displacement, 2) + std::pow(z_displacement, 2)) / (_singleton->_feedrate);
    // printf("time: %lf, x: %lf, y: %lf, z: %lf, feed: %ld\n", time, x_displacement, y_displacement, z_displacement, _singleton->_feedrate);

    if (command.has_x && !command.has_y)
    {
      _singleton->move_x_in_time(x_displacement, time);
    }
    if (command.has_y && !command.has_x)
    {
      _singleton->move_y_in_time(y_displacement, time);
    }
    if (command.has_x && command.has_y)
    {
      _singleton->move_xy_in_time(x_displacement, y_displacement, time);
    }

    if (command.has_z)
    {
      _singleton->move_z_in_time(z_displacement, time);
    }

    if (command.has_ext)
    {
      _singleton->move_e_in_time(e_displacement, time);
    }

    if (command.has_hotbed_temp)
    {
      command.wait = false;
      // _singleton->hotbed_pid.setpoint = 80;
      // _singleton->hotbed_pid.setpoint = command.hotbed_temp;
    }
    if (command.has_hotend_temp)
    {
      command.wait = false;
      // _singleton->hotend_pid.setpoint = command.hotend_temp;
      // _singleton->hotend_pid.setpoint = 240;
    }

    if (command.has_set_position)
    {
      _singleton->stepper_e->set_position(0);
    }

    if (command.wait)
    {
      // printf("%d, %d, %d, %d, %d\n", command.has_x, command.has_y, command.has_z, command.has_ext, command.has_feed_rate);
      while (is_command_running(command))
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }
  }
}

void Printer::printer_task()
{

  while (true)
  {
    std::unique_lock print_lock(_singleton->print_mutex);

    _singleton->start_print.wait(print_lock, []
                                 { return _singleton->print_status == PRINT_STATUS_SETUP; });

    _singleton->print_status = PRINT_STATUS_PRINTING;
    ESP_LOGI(TAG, "Printing %s...", _singleton->filename.c_str());

    FILE *fp = fopen(_singleton->filename.c_str(), "r");
    if (fp == nullptr)
    {
      ESP_LOGE(TAG, "Failed to open file: %s", _singleton->filename.c_str());
      continue;
    }

    char line[128];
    gcode_cmd_t command;

    while (file_get_line(fp, line) > 0)
    {

      if (line[0] == ';')
      {
        continue;
      }

      // printf("%d, %s", _singleton->command_queue.size(), line);
      command = gcode_intrepret_line(line);

      std::unique_lock command_queue_lock(_singleton->command_queue_mutex);
      _singleton->space_available.wait(command_queue_lock, []
                                       { return _singleton->command_queue.size() < PRINTER_COMMAND_QUEUE_SIZE; });

      _singleton->command_queue.push(command);
      _singleton->command_available.notify_one();

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

void Printer::print(std::string const &filename)
{

  ESP_LOGI(TAG, "chegou");

  if (_singleton->print_status == PRINT_STATUS_PRINTING)
  {
    ESP_LOGE(TAG, "Printer is busy printing %s", _singleton->filename.c_str());
    return;
  }

  FILE *fp = fopen(filename.c_str(), "r");
  if (fp == nullptr)
  {
    ESP_LOGE(TAG, "Failed to open file: %s", filename.c_str());
    return;
  }
  fclose(fp);

  _singleton->filename = filename;
  _singleton->print_status = PRINT_STATUS_SETUP;
  _singleton->start_print.notify_one();
}