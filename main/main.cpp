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
#include "http_server.hpp"

#include "esp_settings.hpp"
#include "http_handlers.hpp"
#include "app_info.hpp"

#include "esp_serial.hpp"

#include "i2c.h"
#include "pcf8574.h"

#include "stepper_timer.hpp"
#include "stepper_driver.hpp"

#include "driver/gpio.h"

#include "esp_printer.hpp"

#include "sdcard.h"

#include "main.pb.h"

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
  // Printer::get_instance()->move_z_in_time(100, 2);

  Printer::print("/sdcard/collar_8mm.gcode");


  while (true)
  {
    // ESP_LOGI("mem", "%lu", esp_get_free_heap_size());
    Printer::print_temperatures();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
