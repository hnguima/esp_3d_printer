#include "esp_serial.hpp"

#include <iostream>
#include <thread>

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"

static const char *TAG = "Serial";

std::vector<Serial *> serial_list;

Serial::Serial(int port) : Serial(port, SERIAL_DEFAULT_BAUD_RATE, SERIAL_DEFAULT_DATA_SIZE,
                                  SERIAL_DEFAULT_PARITY, SERIAL_DEFAULT_STOP_BITS,
                                  SERIAL_DEFAULT_FLOW_CONTROL, SERIAL_DEFAULT_MODE) {}

Serial::Serial(int port, uart_mode_t mode) : Serial(port, SERIAL_DEFAULT_BAUD_RATE, SERIAL_DEFAULT_DATA_SIZE,
                                                    SERIAL_DEFAULT_PARITY, SERIAL_DEFAULT_STOP_BITS,
                                                    SERIAL_DEFAULT_FLOW_CONTROL, mode) {}

Serial::Serial(int port, int baud_rate) : Serial(port, baud_rate, SERIAL_DEFAULT_DATA_SIZE,
                                                 SERIAL_DEFAULT_PARITY, SERIAL_DEFAULT_STOP_BITS,
                                                 SERIAL_DEFAULT_FLOW_CONTROL, SERIAL_DEFAULT_MODE) {}

Serial::Serial(int port, int baud_rate, uart_mode_t mode) : Serial(port, baud_rate, SERIAL_DEFAULT_DATA_SIZE,
                                                                   SERIAL_DEFAULT_PARITY, SERIAL_DEFAULT_STOP_BITS,
                                                                   SERIAL_DEFAULT_FLOW_CONTROL, mode) {}

Serial::Serial(int port, int baud_rate, uart_word_length_t data_bits,
               uart_parity_t parity, uart_stop_bits_t stop_bits,
               uart_hw_flowcontrol_t flow_control, uart_mode_t mode) : port(port),
                                                                       baud_rate(baud_rate),
                                                                       data_bits(data_bits),
                                                                       parity(parity),
                                                                       stop_bits(stop_bits),
                                                                       flow_control(flow_control),
                                                                       mode(mode),
                                                                       rx_pin(UART_PIN_NO_CHANGE),
                                                                       tx_pin(UART_PIN_NO_CHANGE),
                                                                       rts_pin(UART_PIN_NO_CHANGE),
                                                                       cts_pin(UART_PIN_NO_CHANGE),
                                                                       on_recv_cb(NULL)
{
  this->config.baud_rate = baud_rate,
  this->config.data_bits = data_bits,
  this->config.parity = parity,
  this->config.stop_bits = stop_bits,
  this->config.flow_ctrl = flow_control,
  this->config.rx_flow_ctrl_thresh = 0,
  this->config.source_clk = UART_SCLK_DEFAULT,

  this->init();
}

Serial::Serial(int port, uart_config_t config) : Serial(port, config, SERIAL_DEFAULT_MODE) {}

Serial::Serial(int port, uart_config_t config, uart_mode_t mode) : port(port),
                                                                   baud_rate(config.baud_rate),
                                                                   data_bits(config.data_bits),
                                                                   parity(config.parity),
                                                                   stop_bits(config.stop_bits),
                                                                   flow_control(config.flow_ctrl),
                                                                   mode(mode),
                                                                   config(config),
                                                                   rx_pin(UART_PIN_NO_CHANGE),
                                                                   tx_pin(UART_PIN_NO_CHANGE),
                                                                   rts_pin(UART_PIN_NO_CHANGE),
                                                                   cts_pin(UART_PIN_NO_CHANGE),
                                                                   on_recv_cb(NULL)
{

  this->init();
}

Serial::~Serial()
{
  ESP_LOGW(TAG, "desctructor called");

  uart_driver_delete(this->port);

  for (size_t i = 0; i < serial_list.size(); i++)
  {
    if (this->port == serial_list.at(i)->port)
    {
      serial_list.erase(serial_list.begin() + i);
    }
  }
}

void Serial::init()
{

  for (Serial *serial : serial_list)
  {
    if (serial->port == port)
    {
      ESP_LOGE(TAG, "there is already a serial started in this port");
      return;
    }
  }

  this->mutex = xSemaphoreCreateMutex();

  if (this->mutex == NULL)
  {
    ESP_LOGE(TAG, "nao foi possivel criar o mutex");
    return;
  }

  ESP_ERROR_CHECK(uart_param_config(this->port, &(this->config)));

  // Set UART pins (using UART0 default pins ie no changes.)
  ESP_ERROR_CHECK(uart_set_pin(this->port, this->rx_pin, this->tx_pin, this->rts_pin, this->cts_pin));

  ESP_ERROR_CHECK(uart_driver_install(this->port, SERIAL_RX_BUFFER_SIZE * 2, SERIAL_TX_BUFFER_SIZE * 2, 20, &(this->queue), 0));

  ESP_ERROR_CHECK(uart_set_mode(this->port, this->mode));

  sprintf(this->task_name, "Serial[%d]", this->port);

  // Create a task to handler UART event from ISR
  xTaskCreate(Serial::task, this->task_name, 4096, (void *)this, 12, NULL);

  serial_list.push_back(this);
}

void Serial::task(void *param)
{

  Serial *serial_instance = (Serial *)param;

  uart_event_t event;
  uint8_t *data = (uint8_t *)malloc(SERIAL_RX_BUFFER_SIZE);

  ESP_LOGI(serial_instance->task_name, "Serial port: %d", serial_instance->port);

  for (;;)
  {
    // Waiting for UART event.
    if (xQueueReceive(serial_instance->queue, (void *)&event, portMAX_DELAY))
    {
      // bzero(data, SERIAL_RX_BUFFER_SIZE);
      // ESP_LOGI(serial_instance->task_name, "uart[%d] event:", serial_instance->port);

      switch (event.type)
      {
      // Event of UART receving data
      /*We'd better handler data event fast, there would be much more data events than
      other types of events. If we take too much time on data event, the queue might
      be full.*/
      case UART_DATA:
        uart_read_bytes(serial_instance->port, data, event.size, portMAX_DELAY);

        // ESP_LOG_BUFFER_HEXDUMP(serial_instance->task_name, data, event.size, ESP_LOG_INFO);

        if (serial_instance->on_recv_cb)
        {
          serial_instance->on_recv_cb(data, (uint32_t &)event.size);
        }

        break;
      // Event of HW FIFO overflow detected
      case UART_FIFO_OVF:
        ESP_LOGI(serial_instance->task_name, "hw fifo overflow");
        // If fifo overflow happened, you should consider adding flow control for your application.
        // The ISR has already reset the rx FIFO,
        // As an example, we directly flush the rx buffer here in order to read more data.
        uart_flush_input(serial_instance->port);
        xQueueReset(serial_instance->queue);
        break;
      // Event of UART ring buffer full
      case UART_BUFFER_FULL:
        ESP_LOGI(serial_instance->task_name, "ring buffer full");
        // If buffer full happened, you should consider increasing your buffer size
        // As an example, we directly flush the rx buffer here in order to read more data.
        uart_flush_input(serial_instance->port);
        xQueueReset(serial_instance->queue);
        break;
      // Event of UART RX break detected
      case UART_BREAK:
        ESP_LOGI(serial_instance->task_name, "uart rx break");
        break;
      // Event of UART parity check error
      case UART_PARITY_ERR:
        ESP_LOGI(serial_instance->task_name, "uart parity error");
        break;
      // Event of UART frame error
      case UART_FRAME_ERR:
        ESP_LOGI(serial_instance->task_name, "uart frame error");
        break;
      // UART_PATTERN_DET
      case UART_PATTERN_DET:
        // TODO: implement a way to register patterns for detection
        break;
      // Others
      default:
        ESP_LOGI(serial_instance->task_name, "uart event type: %d", event.type);
        break;
      }
    }
  }
  free(data);
  data = NULL;
  vTaskDelete(NULL);
}

esp_err_t Serial::set_pins(int rx_pin, int tx_pin)
{
  return set_pins(rx_pin, tx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

esp_err_t Serial::set_pins(int rx_pin, int tx_pin, int rts_pin, int cts_pin)
{
  this->rx_pin = rx_pin;
  this->tx_pin = tx_pin;
  this->rts_pin = rts_pin;
  this->cts_pin = cts_pin;

  ESP_LOGI(TAG, "uart num: %d", this->port);

  return uart_set_pin(this->port, this->rx_pin, this->tx_pin, this->rts_pin, this->cts_pin);
}

void Serial::on_recv(recv_handler_func_t func)
{
  this->on_recv_cb = func;
}
esp_err_t Serial::send_bytes(uint8_t *buffer, uint32_t size)
{
  esp_err_t err = ESP_OK;

  if (xSemaphoreTake(this->mutex, (TickType_t)100) != pdTRUE)
  {
    ESP_LOGE(TAG, "mutex socket nao liberado");
    return ESP_FAIL;
  }

  // else if (this->size_tx != 0)
  // {
  //   ESP_LOGE(TAG, "client fd is 0");
  //   return ESP_ERR_INVALID_STATE;
  // }
  if (size > SERIAL_TX_BUFFER_SIZE)
  {
    ESP_LOGE(TAG, "send data greater than %d", SERIAL_TX_BUFFER_SIZE);
    return ESP_ERR_INVALID_ARG;
  }

  memcpy(this->buffer_tx, buffer, size);
  this->size_tx = size;

  err = uart_write_bytes(this->port, this->buffer_tx, this->size_tx);

  if (err < 0)
  {
    ESP_LOGE(TAG, "error on send");
  }

  this->size_tx = 0;
  xSemaphoreGive(this->mutex);

  return err;
}
