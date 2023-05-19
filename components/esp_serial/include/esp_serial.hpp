#pragma once

#include <functional>
#include <thread>

#include <esp_err.h>
#include <driver/uart.h>

#include "freertos/queue.h"

#define SERIAL_DEFAULT_BAUD_RATE 115200
#define SERIAL_DEFAULT_DATA_SIZE UART_DATA_8_BITS
#define SERIAL_DEFAULT_PARITY UART_PARITY_DISABLE
#define SERIAL_DEFAULT_STOP_BITS UART_STOP_BITS_1
#define SERIAL_DEFAULT_FLOW_CONTROL UART_HW_FLOWCTRL_DISABLE
#define SERIAL_DEFAULT_MODE UART_MODE_UART

#define SERIAL_RX_BUFFER_SIZE 256
#define SERIAL_TX_BUFFER_SIZE 256

class Serial
{

public:
  Serial(int port);
  Serial(int port, uart_mode_t mode);
  Serial(int port, int baud_rate);
  Serial(int port, int baud_rate, uart_mode_t mode);
  Serial(int port, int baud_rate, uart_word_length_t data_bits,
         uart_parity_t parity, uart_stop_bits_t stop_bits,
         uart_hw_flowcontrol_t flow_control, uart_mode_t mode);
  Serial(int port, uart_config_t config);
  Serial(int port, uart_config_t config, uart_mode_t mode);

  ~Serial();

  esp_err_t set_pins(int rx_pin, int tx_pin);
  esp_err_t set_pins(int rx_pin, int tx_pin, int rts_pin, int cts_pin);

  typedef std::function<void(uint8_t *, uint32_t &)> recv_handler_func_t;
  void on_recv(recv_handler_func_t func);
  esp_err_t send_bytes(uint8_t *buf, uint32_t size);

private:
  int port;
  int baud_rate;
  uart_word_length_t data_bits;
  uart_parity_t parity;
  uart_stop_bits_t stop_bits;
  uart_hw_flowcontrol_t flow_control;
  uart_mode_t mode;

  uart_config_t config;

  QueueHandle_t queue;

  // pinout
  int rx_pin, tx_pin, rts_pin, cts_pin;

  recv_handler_func_t on_recv_cb;
  // uint8_t rx_buffer[];
  uint8_t buffer_rx[SERIAL_RX_BUFFER_SIZE];
  int16_t size_rx;
  uint8_t buffer_tx[SERIAL_TX_BUFFER_SIZE];
  int16_t size_tx;

  char task_name[16];

  SemaphoreHandle_t mutex;

  void init();
  static void task(void *param);
};
