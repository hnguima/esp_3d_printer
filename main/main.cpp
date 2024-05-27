#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <string>
#include <sstream>
#include <esp_pthread.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "stepper_driver.hpp"
#include "esp_filesystem.hpp"

#include "esp_wifi_driver.hpp"
#include "firmware_update.hpp"
// #include "http_server.hpp"

#include "esp_settings.hpp"
// #include "http_handlers.hpp"

#include "esp_serial.hpp"

#include "i2c.h"
#include "pcf8574.h"

#include "stepper_timer.hpp"
#include "stepper_driver.hpp"

#include "driver/gpio.h"

#include "esp_printer.hpp"

#include "sdcard.h"


extern "C" void app_main()
{

  FileSystem *file_system = FileSystem::get_instance();
  file_system->mount_all();

  // auto *main_settings = new Settings<MainConfig>("/data/main.set");
  // init_app_info_updater(main_settings);

  // WifiDriver *wifi_driver = WifiDriver::get_instance();
  // wifi_driver->init_STA("CLARO_2GA8652A", "38A8652A");

  // HTTPServer::init();
  // HTTPServer *server = HTTPServer::get_instance();
  // server->add_user(main_settings->obj->login.username,
  //                  main_settings->obj->login.password);

  // init_settings_http_handler(main_settings);

  // FirmwareUpdate fw_update;



  sdcard_init();

  Printer::init();
  Printer::print("/sdcard/TacoLixa430_2.gcode");

  Printer *printer = Printer::get_instance();


  while (true)
  {
    // ESP_LOGI("mem", "%lu", esp_get_free_heap_size());
    // ESP_LOGI("test","Targ x: %lf y: %lf z: %lf | Curr x: %lf y: %lf z: %lf", printer->desired_x, printer->desired_y, printer->desired_z, printer->x_pos, printer->y_pos, printer->z_pos);
    // ESP_LOGI("test", "Deviation x: %lf y: %lf z: %lf", printer->desired_x - printer->x_pos, printer->desired_y - printer->y_pos, printer->desired_z - printer->z_pos);
    // Printer::print_temperatures();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
