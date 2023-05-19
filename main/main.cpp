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
#include "esp_ethernet.hpp"
#include "firmware_update.hpp"
#include "http_server.hpp"
#include "ws_log.hpp"

#include "esp_settings.hpp"
#include "http_handlers.hpp"
#include "app_info.hpp"

#include "socket_server_tcp.hpp"

#include "esp_serial.hpp"

#include "modbus_gateway.hpp"
#include "mdb_relay_io.h"
#include "test.hpp"

#include "main.pb.h"

extern "C" void app_main()
{
  test_module_init();

  mdb_relay_io_init();

  FileSystem *file_system = FileSystem::get_instance();
  file_system->mount_all();

  Settings<MainConfig> *main_settings = new Settings<MainConfig>("/data/main.set");
  init_app_info_updater(main_settings);

  Ethernet::init();

  if (main_settings->obj->wifi.enabled)
  {
    WifiDriver *wifi_driver = WifiDriver::get_instance();
    wifi_driver->init_STA(main_settings->obj->wifi.ssid,
                          main_settings->obj->wifi.password);
  }
  // wifi_driver->init_STA("CLARO_2GA8652A", "38A8652A");

  HTTPServer::init();
  HTTPServer *server = HTTPServer::get_instance();
  server->add_user(main_settings->obj->login.username,
                   main_settings->obj->login.password);

  init_settings_http_handler(main_settings);

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  WebSocketLog::init();

  FirmwareUpdate fw_update;

  if (main_settings->obj->channel_1.enabled)
  {
    ModbusGatewayConfig *temp = &main_settings->obj->channel_1;

    ModbusGateway *mdb_gw_1 = new ModbusGateway();

    mdb_gw_1->config_rtu(UART_NUM_1, temp->rtu_baud_rate, (uart_parity_t)temp->rtu_parity,
                         (uart_stop_bits_t)temp->rtu_stop_bits, (uart_word_length_t)temp->rtu_data_bits);
    mdb_gw_1->set_rtu_timeout(temp->rtu_timeout);
    mdb_gw_1->set_rtu_pins(2, 4);

    mdb_gw_1->set_tcp_timeout(temp->tcp_timeout);
    mdb_gw_1->config_tcp(temp->tcp_port);
  }

  if (main_settings->obj->channel_2.enabled)
  {
    ModbusGatewayConfig *temp = &main_settings->obj->channel_2;

    ModbusGateway *mdb_gw_2 = new ModbusGateway();
    mdb_gw_2->config_rtu(UART_NUM_2, temp->rtu_baud_rate, (uart_parity_t)temp->rtu_parity,
                         (uart_stop_bits_t)temp->rtu_stop_bits, (uart_word_length_t)temp->rtu_data_bits);
    mdb_gw_2->set_rtu_timeout(temp->rtu_timeout);

    mdb_gw_2->set_tcp_timeout(temp->tcp_timeout);
    mdb_gw_2->config_tcp(temp->tcp_port);
  }

  while (true)
  {
    ESP_LOGI("mem", "%lu", esp_get_free_heap_size());
    // ESP_LOGI("main", "sntp sync status: %d", sntp_get_sync_status());

    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}
