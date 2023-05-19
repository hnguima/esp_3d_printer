#pragma once

#include <vector>
#include <string.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_timer.h>

#include "socket_server_tcp.hpp"
#include "esp_serial.hpp"

#define MODBUS_GATEWAY_MAX_TCP_RETRIES 5
#define MODBUS_GATEWAY_MAX_RTU_RETRIES 5

class ModbusConverter;
class ModbusGateway
{

public:
  ModbusGateway();

  void config_tcp(uint16_t port);

  void config_rtu(uint32_t uart_port);
  void config_rtu(uint32_t uart_port, uint32_t baud_rate);
  void config_rtu(uint32_t uart_port, uint32_t baud_rate,
                  uart_parity_t parity, uart_stop_bits_t stop_bits,
                  uart_word_length_t data_bits);

  esp_err_t set_rtu_pins(int rx_pin, int tx_pin);
  esp_err_t set_rtu_pins(int rx_pin, int tx_pin, int rts_pin, int cts_pin);

  void set_rtu_timeout(uint32_t timeout);
  void set_tcp_timeout(uint32_t timeout);

private:
  SocketServer *server;
  uint16_t tcp_port;
  uint32_t tcp_timeout;
  uint8_t tcp_data[256];
  uint16_t tcp_size;
  uint8_t tcp_retries;
  int client;

  bool tcp_config_ok;

  Serial *serial;
  uint32_t rtu_uart_port;
  uint32_t rtu_baud_rate;
  uart_parity_t rtu_parity;
  uart_stop_bits_t rtu_stop_bits;
  uart_word_length_t rtu_data_bits;
  uint8_t rtu_retries;
  uint32_t rtu_timeout;

  uint8_t rtu_data[256];
  uint16_t rtu_size;

  bool rtu_config_ok;

  ModbusConverter *modbus_converter;
  esp_timer_handle_t tcp_watchdog;
  esp_timer_handle_t rtu_watchdog;

  static void tcp_watchdog_handler(void *);
  static void rtu_watchdog_handler(void *);

  static std::vector<ModbusGateway *> gateways;

  void init_rtu(uint32_t uart_port, uint32_t baud_rate,
                uart_parity_t parity, uart_stop_bits_t stop_bits,
                uart_word_length_t data_bits);

  void wait_for_config();

  char task_name[32];
  static void task(void *param);
};
