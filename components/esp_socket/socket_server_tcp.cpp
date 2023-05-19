#include "socket_server_tcp.hpp"

#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_event.h>
#include <esp_netif.h>

#define MAX_CLIENTS 10

static const char *TAG = "SocketServer";

std::vector<SocketServer *> SocketServer::open_sockets;
std::vector<int> _clients;

SocketServer::SocketServer(uint16_t port) : Socket(),
                                            port(port),
                                            on_client_connect_cb(NULL),
                                            on_client_disconnect_cb(NULL),
                                            on_client_recv_cb(NULL)
{

  if (!(this->open_sockets.size() < MAX_SOCKETS))
  {
    ESP_LOGE(TAG, "Numero de servers abertos ja é o maximo permitido: %d", MAX_SOCKETS);
    return;
  }

  for (SocketServer *socket : this->open_sockets)
  {
    if (this == socket)
    {
      ESP_LOGE(TAG, "Já existe um servidor aberto com este IP e porta");
      return;
    }
  }

  this->open_sockets.push_back(this);
  ESP_LOGI(TAG, "SocketServers abertos: %d", this->open_sockets.size());

  // char task_name[32];

  this->task_handle = (TaskHandle_t *)malloc(sizeof(TaskHandle_t));
  xTaskCreate(SocketServer::task, this->task_name, 4096, (void *)this, 5, this->task_handle);
}

void SocketServer::delete_task()
{
  Socket::delete_task();

  this->open_sockets.erase(
      std::remove(this->open_sockets.begin(), this->open_sockets.end(), this),
      this->open_sockets.end());
}

bool SocketServer::operator==(SocketServer const &rhs)
{
  return port == rhs.port;
}

esp_err_t SocketServer::on_client_connect(client_handler_func_t func)
{
  this->on_client_connect_cb = func;
  return ESP_OK;
}

esp_err_t SocketServer::on_client_disconnect(client_handler_func_t func)
{
  this->on_client_disconnect_cb = func;
  return ESP_OK;
}

esp_err_t SocketServer::on_client_recv(client_recv_handler_func_t func)
{
  this->on_client_recv_cb = func;
  return ESP_OK;
}

int SocketServer::bind_socket()
{

  // Vincula o IP do servidor com a PORTA a ser aberta.
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = ipaddr_addr(Socket::ip);
  server_addr.sin_port = htons(this->port);

  return bind(this->fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
}

void SocketServer::task(void *param)
{

  SocketServer *socket_instance = (SocketServer *)param;

  // Wait for network
  socket_instance->wait_for_network();
  vTaskDelay(50 / portTICK_PERIOD_MS);

  ESP_LOGI(TAG, "Socket ip: %s", Socket::ip);

  sprintf(socket_instance->task_name, "socket(%s:%d)", Socket::ip, socket_instance->port);

  while (true)
  {

    // Initialize socket
    socket_instance->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_instance->fd < 0)
    {
      ESP_LOGE(socket_instance->task_name, "error (%d) while starting socket", socket_instance->fd);
      perror("socket");

      RETURN_OR_CONTINUE(socket_instance->retry() == 0);
    }

    // bind server socket to address
    int err = socket_instance->bind_socket();
    if (err < 0)
    {
      ESP_LOGE(TAG, "error (%d) while starting socket.", err);
      perror("bind");

      RETURN_OR_CONTINUE(socket_instance->retry() == 0);
    }

    ESP_LOGI(socket_instance->task_name, "socket server binded");

    //  socket->wd_mdb_counter = 0; //Zera o contador do watchdog da conexão Modbus TCP

    // Adiciona a task atual a lista de tasks monitoradas por watchdog
    //  esp_task_wdt_add(NULL);
    // start listening/accepting task
    // define max number of listening clients
    err = listen(socket_instance->fd, MAX_CLIENTS);
    if (err < 0)
    {
      ESP_LOGE(TAG, "error (%d) while starting listening.", err);
      perror("listen");

      RETURN_OR_CONTINUE(socket_instance->retry() == 0);
    }

    socket_instance->retries = 0;

    if (socket_instance->server_loop() == 0)
    {
      return;
    }
  }
}

bool SocketServer::server_loop()
{

  struct sockaddr_in address;
  int address_size = sizeof(address);

  int max_fd;
  int activity;
  int new_client;

  while (true)
  {
    // clear the socket set
    FD_ZERO(&this->all_fds);

    // add master socket to set
    FD_SET(this->fd, &this->all_fds);
    max_fd = this->fd;

    // add child sockets to set
    for (int socket_fd : this->clients)
    {
      // if valid socket descriptor then add to read list
      if (socket_fd > 0)
        FD_SET(socket_fd, &this->all_fds);

      // highest file descriptor number, need it for the select function
      if (socket_fd > max_fd)
        max_fd = socket_fd;
    }

    // wait for an activity on one of the sockets , timeout is NULL ,
    // so wait indefinitely
    activity = select(max_fd + 1, &this->all_fds, NULL, NULL, NULL);

    if ((activity < 0) && (errno != EINTR))
    {
      ESP_LOGE(this->task_name, "select error (%d)", errno);
      perror("select");
    }

    // If something happened on the master socket ,
    // then its an incoming connection
    if (FD_ISSET(this->fd, &this->all_fds))
    {
      new_client = accept(this->fd, (struct sockaddr *)&address, (socklen_t *)&address_size);
      if (new_client < 0)
      {
        ESP_LOGE(this->task_name, "accept error (%d)", errno);
        perror("accept");
        RETURN_OR_BREAK(this->server_loop() == 0)
      }

      // inform user of socket number - used in send and receive commands
      ESP_LOGI(this->task_name, "client connected: %s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

      this->clients.push_back(new_client);

      if (this->on_client_connect_cb)
      {
        this->on_client_connect_cb(new_client);
      }
    }

    // add child sockets to set
    for (int socket_fd : this->clients)
    {

      if (FD_ISSET(socket_fd, &this->all_fds))
      {
        // Check if it was for closing , and also read the
        // incoming message
        this->size_rx = read(socket_fd, this->buffer_rx, 1024);

        if (this->size_rx <= 0)
        {
          ESP_LOGE(this->task_name, "error (%d) on recv", errno);
          perror("recv");
          // Somebody disconnected , get his details and print
          getpeername(socket_fd, (struct sockaddr *)&address, (socklen_t *)&address_size);
          ESP_LOGW(this->task_name, "client disconnected: %s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

          // Close the socket and mark as 0 in list for reuse
          close(socket_fd);
          this->clients.erase(
              std::remove(this->clients.begin(), this->clients.end(), socket_fd),
              this->clients.end());

          if (this->on_client_disconnect_cb)
          {
            this->on_client_disconnect_cb(socket_fd);
          }

          break;
        }

        this->retries = 0;

        // ESP_LOG_BUFFER_HEXDUMP(this->task_name, this->buffer_rx, this->size_rx, ESP_LOG_WARN);

        this->buffer_rx[this->size_rx] = '\0';
        if (this->on_client_recv_cb)
        {
          this->on_client_recv_cb(socket_fd, this->buffer_rx, this->size_rx);
        }
      }
    }
  }

  return true;
}
