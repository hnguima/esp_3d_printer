#include "socket_client_tcp.hpp"

#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_event.h>
#include <esp_netif.h>

static const char *TAG = "SocketClient";

std::vector<SocketClient *> SocketClient::open_sockets;

SocketClient::SocketClient(std::string addr, uint16_t port) : Socket(),
                                                              addr(addr),
                                                              port(port),
                                                              on_connect_cb(NULL),
                                                              on_disconnect_cb(NULL)
{

  if (!(this->open_sockets.size() < MAX_SOCKETS))
  {
    ESP_LOGE(TAG, "Numero de servers abertos ja é o maximo permitido: %d", MAX_SOCKETS);
    return;
  }

  for (SocketClient *socket : this->open_sockets)
  {
    if (this == socket)
    {
      ESP_LOGE(TAG, "Já existe um servidor aberto com este IP e porta");
      return;
    }
  }

  this->open_sockets.push_back(this);
  ESP_LOGI(TAG, "SocketClients abertos: %d", this->open_sockets.size());

  // char task_name[32];
  sprintf(this->task_name, "socket(%s:%d)", this->addr.c_str(), this->port);

  this->task_handle = (TaskHandle_t *)malloc(sizeof(TaskHandle_t));
  xTaskCreate(SocketClient::task, this->task_name, 4096, (void *)this, 5, this->task_handle);
}

void SocketClient::delete_task()
{
  Socket::delete_task();

  this->open_sockets.erase(
      std::remove(this->open_sockets.begin(), this->open_sockets.end(), this),
      this->open_sockets.end());
}

bool SocketClient::operator==(SocketClient const &rhs)
{
  return addr == rhs.addr && port == rhs.port;
}

bool SocketClient::retry()
{
  close(this->fd);

  if (this->on_disconnect_cb)
  {
    this->on_disconnect_cb();
  }

  return Socket::retry();
}

esp_err_t SocketClient::on_connect(handler_func_t func)
{
  this->on_connect_cb = func;
  return ESP_OK;
}

esp_err_t SocketClient::on_disconnect(handler_func_t func)
{
  this->on_disconnect_cb = func;
  return ESP_OK;
}

int SocketClient::connect_socket()
{

  // Vincula o IP do servidor com a PORTA a ser aberta.
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = ipaddr_addr(this->addr.c_str());
  server_addr.sin_port = htons(this->port);

  // Aceita a conexão e retorna o socket do cliente "clientSock" armazenará o handle da conexão socket do cliente
  return connect(this->fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
}

void SocketClient::task(void *param)
{

  SocketClient *socket_instance = (SocketClient *)param;

  while (true)
  {
    // Wait for network
    socket_instance->wait_for_network();

    // Initialize socket
    socket_instance->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_instance->fd < 0)
    {
      ESP_LOGE(socket_instance->task_name, "error (%d) while starting socket", socket_instance->fd);
      perror("socket");

      RETURN_OR_CONTINUE(socket_instance->retry() == 0);
    }

    // Accepts server connection
    int err = socket_instance->connect_socket();
    if (err < 0)
    {
      ESP_LOGE(socket_instance->task_name, "error (%d) while connecting to server", err);
      perror("connect");

      RETURN_OR_CONTINUE(socket_instance->retry() == 0);
    }

    ESP_LOGI(socket_instance->task_name, "socket client conected");

    if (socket_instance->on_connect_cb)
    {
      socket_instance->on_connect_cb();
    }

    socket_instance->retries = 0;

    //  socket->wd_mdb_counter = 0; //Zera o contador do watchdog da conexão Modbus TCP

    // Adiciona a task atual a lista de tasks monitoradas por watchdog
    //  esp_task_wdt_add(NULL);

    if (socket_instance->recv_loop() == 0)
    {
      return;
    }
  }
}
