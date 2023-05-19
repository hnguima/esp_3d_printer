#include "modbus_gateway.hpp"
#include "modbus_converter.hpp"

static const char *TAG = "ModbusGateway";

std::vector<ModbusGateway *> ModbusGateway::gateways;

ModbusGateway::ModbusGateway() : server(nullptr),
                                 tcp_port(0),
                                 tcp_timeout(portMAX_DELAY),
                                 tcp_retries(0),
                                 tcp_config_ok(false),
                                 serial(nullptr),
                                 rtu_uart_port(0),
                                 rtu_baud_rate(SERIAL_DEFAULT_BAUD_RATE),
                                 rtu_parity(SERIAL_DEFAULT_PARITY),
                                 rtu_stop_bits(SERIAL_DEFAULT_STOP_BITS),
                                 rtu_data_bits(SERIAL_DEFAULT_DATA_SIZE),
                                 rtu_retries(0),
                                 rtu_timeout(portMAX_DELAY),
                                 rtu_config_ok(false)
{

  // for (ModbusGateway gateway : ModbusGateway::gateways)
  // {
  //   if (gateway.rtu_uart_port == this->rtu_uart_port)
  //   {
  //   }

  //   if (gateway.tcp_port == this->tcp_port)
  //   {
  //   }
  // }
  const esp_timer_create_args_t tcp_watchdog_args = {
      .callback = ModbusGateway::tcp_watchdog_handler,
      .arg = this,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "tcp_watchdog",
      .skip_unhandled_events = true,
  };
  esp_timer_create(&tcp_watchdog_args,
                   &this->tcp_watchdog);

  const esp_timer_create_args_t rtu_watchdog_args = {
      .callback = ModbusGateway::rtu_watchdog_handler,
      .arg = this,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "rtu_watchdog",
      .skip_unhandled_events = true,
  };
  esp_timer_create(&rtu_watchdog_args,
                   &this->rtu_watchdog);

  sprintf(this->task_name, "mdb_gw(%d)", ModbusGateway::gateways.size());
  xTaskCreate(ModbusGateway::task, this->task_name, 4096, (void *)this, 5, NULL);
}

void ModbusGateway::config_tcp(uint16_t port)
{
  for (ModbusGateway *gateway : ModbusGateway::gateways)
  {
    if (gateway->tcp_port == this->tcp_port)
    {
      ESP_LOGE(TAG, "there is already a gateway open on tcp port %d", this->tcp_port);
      return;
    }
  }

  this->server = new SocketServer(port);

  this->tcp_config_ok = true;
}

void ModbusGateway::config_rtu(uint32_t uart_port)
{

  this->init_rtu(uart_port, SERIAL_DEFAULT_BAUD_RATE, SERIAL_DEFAULT_PARITY,
                 SERIAL_DEFAULT_STOP_BITS, SERIAL_DEFAULT_DATA_SIZE);
}
void ModbusGateway::config_rtu(uint32_t uart_port, uint32_t baud_rate)
{
  this->init_rtu(uart_port, baud_rate, SERIAL_DEFAULT_PARITY,
                 SERIAL_DEFAULT_STOP_BITS, SERIAL_DEFAULT_DATA_SIZE);
}

void ModbusGateway::config_rtu(uint32_t uart_port, uint32_t baud_rate,
                               uart_parity_t parity, uart_stop_bits_t stop_bits,
                               uart_word_length_t data_bits)
{
  this->init_rtu(uart_port, baud_rate, parity, stop_bits, data_bits);
}

void ModbusGateway::init_rtu(uint32_t uart_port, uint32_t baud_rate,
                             uart_parity_t parity, uart_stop_bits_t stop_bits,
                             uart_word_length_t data_bits)
{

  for (ModbusGateway *gateway : ModbusGateway::gateways)
  {
    if (gateway->rtu_uart_port == this->rtu_uart_port)
    {
      ESP_LOGE(TAG, "there is already a gateway open on uart port %lu", this->rtu_uart_port);
      return;
    }
  }

  this->serial = new Serial(uart_port, baud_rate, data_bits, parity, stop_bits,
                            SERIAL_DEFAULT_FLOW_CONTROL, UART_MODE_RS485_HALF_DUPLEX);

  this->rtu_uart_port = uart_port;
  this->rtu_baud_rate = baud_rate;
  this->rtu_parity = parity;
  this->rtu_stop_bits = stop_bits;
  this->rtu_data_bits = data_bits;

  this->rtu_config_ok = true;
}

esp_err_t ModbusGateway::set_rtu_pins(int rx_pin, int tx_pin)
{
  return this->serial->set_pins(rx_pin, tx_pin);
}
esp_err_t ModbusGateway::set_rtu_pins(int rx_pin, int tx_pin, int rts_pin, int cts_pin)
{
  return this->serial->set_pins(rx_pin, tx_pin, rts_pin, cts_pin);
}

void ModbusGateway::set_rtu_timeout(uint32_t timeout)
{
  this->rtu_timeout = timeout;
}
void ModbusGateway::set_tcp_timeout(uint32_t timeout)
{
  this->tcp_timeout = timeout;
}

void ModbusGateway::wait_for_config()
{
  while (this->rtu_config_ok == false && this->tcp_config_ok == false)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  ModbusGateway::gateways.push_back(this);
}

void ModbusGateway::tcp_watchdog_handler(void *param)
{

  ModbusGateway *gateway = (ModbusGateway *)param;

  ESP_LOGW(gateway->task_name, "WTD called on tcp, didnt receive tcp request for the last %d tries", gateway->tcp_retries++);

  if (gateway->tcp_retries > MODBUS_GATEWAY_MAX_TCP_RETRIES)
  {
    esp_restart();
  }
}

void ModbusGateway::rtu_watchdog_handler(void *param)
{
  ModbusGateway *gateway = (ModbusGateway *)param;

  ESP_LOGW(gateway->task_name, "WTD called on rtu, didnt receive rtu response for the last %d tries", gateway->rtu_retries++);

  if (gateway->tcp_retries > MODBUS_GATEWAY_MAX_RTU_RETRIES)
  {
    esp_restart();
  }

  gateway->serial->send_bytes(gateway->rtu_data, gateway->rtu_size);
}

void ModbusGateway::task(void *param)
{
  ModbusGateway *gateway = (ModbusGateway *)param;

  gateway->wait_for_config();

  gateway->modbus_converter = new ModbusConverter();

  esp_timer_start_periodic(gateway->tcp_watchdog, gateway->tcp_timeout * 1000 * 1000);

  gateway->server->on_client_recv(
      [gateway](int socket_client, uint8_t *data, size_t size)
      {
        gateway->client = socket_client;
        esp_timer_stop(gateway->tcp_watchdog);
        esp_timer_start_periodic(gateway->tcp_watchdog, gateway->tcp_timeout * 1000 * 1000);

        gateway->modbus_converter->set_tcp_packet(data, size);

        gateway->modbus_converter->get_rtu_packet((uint8_t *)gateway->rtu_data, &gateway->rtu_size);

        // ESP_LOG_BUFFER_HEXDUMP("tcp sending", rtu_data, rtu_size, ESP_LOG_INFO);

        gateway->serial->send_bytes(gateway->rtu_data, gateway->rtu_size);

        esp_timer_start_periodic(gateway->rtu_watchdog, gateway->rtu_timeout * 1000);
      });

  gateway->serial->on_recv(
      [gateway](uint8_t *data, uint32_t size)
      {
        esp_timer_stop(gateway->rtu_watchdog);
        // ESP_LOG_BUFFER_HEXDUMP("rtu recv", data, size, ESP_LOG_INFO);

        gateway->modbus_converter->set_rtu_packet(data, size);

        gateway->modbus_converter->get_tcp_packet((uint8_t *)gateway->tcp_data, &gateway->tcp_size);

        // ESP_LOG_BUFFER_HEXDUMP("rtu sending", tcp_data, tcp_size, ESP_LOG_INFO);

        gateway->server->send_bytes(gateway->client, (uint8_t *)gateway->tcp_data, gateway->tcp_size);

        // gateway->client = 0;
      });

  while (true)
  {
    // do something
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}
