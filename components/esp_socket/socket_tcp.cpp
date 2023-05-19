#include "socket_tcp.hpp"

#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_event.h>
#include <esp_netif.h>

static const char *TAG = "Socket";

bool Socket::has_network = false;
char Socket::ip[16] = "";

Socket::Socket() : on_recv_cb(NULL),
                   is_persistent(true),
                   is_vital(false),
                   retries(0),
                   max_retries(0),
                   retry_delay(SOCKET_RETRY_INTERVAL)
{

  this->mutex = xSemaphoreCreateMutex();

  if (this->mutex == NULL)
  {
    ESP_LOGE(TAG, "nao foi possivel criar o mutex");
    return;
  }
}

void Socket::delete_task()
{
  // Falha na abertura do socket Server, ou por falta de memória ou IP errado do servidor
  ESP_LOGI(TAG, "deleging socket task...");

  esp_err_t err = close(this->fd);
  ESP_LOGI(TAG, "error (%d) closing the socket", err);
  perror("close");

  // Deleting the current task
  vTaskDelete(*this->task_handle);

}

bool Socket::recv_loop()
{
  while (true)
  {
    this->size_rx = recv(this->fd, this->buffer_rx, 1024, 0);

    if (this->size_rx <= 0)
    {
      ESP_LOGE(this->task_name, "error (%d) on recv", errno);
      perror("recv");

      // Deleta a task atual da lista de tasks monitoradas por watchdog
      //  esp_task_wdt_delete(NULL);

      // break the receiving loop and retry connection;

      RETURN_OR_BREAK(this->retry() == 0);
    }

    this->retries = 0;

    // ESP_LOG_BUFFER_HEXDUMP(this->task_name, this->buffer_rx, this->size_rx, ESP_LOG_WARN);

    this->buffer_rx[this->size_rx] = '\0';
    if (this->on_recv_cb)
    {
      this->on_recv_cb(this->buffer_rx, this->size_rx);
    }

    // Reseta o watchdog, caso não tenha travado
    //  esp_task_wdt_reset();
    //  Bloqueia a task por alguns instantes.
    //  Caso seja necessario maior throughput de dados, diminua esse intervalo.
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  return true;
}

bool Socket::retry()
{

  if (this->is_persistent)
  {

    ESP_LOGW(this->task_name, "retrying in %.3fs, retry attempt: %d.", (float)this->retry_delay / 1000, ++this->retries);
    vTaskDelay(this->retry_delay / portTICK_PERIOD_MS);

    return true;
  }

  if (this->retries < this->max_retries)
  {

    ESP_LOGW(this->task_name, "retrying in %.3fs, retry attempt: %d of %d.", (float)this->retry_delay / 1000, ++this->retries, this->max_retries);
    vTaskDelay(this->retry_delay / portTICK_PERIOD_MS);

    return true;
  }

  this->delete_task();

  if (this->is_vital)
  {
    ESP_LOGE(this->task_name, "could not connect, max retries achieved. Restarting device in %ds...", 2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    ESP_LOGI(this->task_name, "restarting...");

    esp_restart();
  }

  return false;
}

std::vector<esp_netif_t *> get_netif_list()
{
  std::vector<esp_netif_t *> netif_list;

  esp_netif_t *netif_it = NULL;

  do
  {
    netif_it = esp_netif_next(netif_it);
    netif_list.push_back(netif_it);

  } while (netif_it != NULL);

  return netif_list;
}

void Socket::wait_for_network()
{

  while (Socket::has_network == false)
  {
    std::vector<esp_netif_t *> netif_list = get_netif_list();

    if (netif_list.size() != 0)
    {
      for (esp_netif_t *netif_item : netif_list)
      {
        esp_netif_ip_info_t ip_info;
        esp_netif_get_ip_info(netif_item, &ip_info);

        if (ip_info.ip.addr != 0 && esp_netif_is_netif_up(netif_item))
        {
          Socket::has_network = true;
          sprintf(Socket::ip, IPSTR, IP2STR(&ip_info.ip));
          Socket::ip[strlen(Socket::ip)] = '\0';
        }
      }
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

esp_err_t Socket::on_recv(recv_handler_func_t func)
{
  this->on_recv_cb = func;
  return ESP_OK;
}

bool Socket::send_bytes(int client_fd, const uint8_t *buffer, size_t size)
{

  esp_err_t err = ESP_OK;

  if (xSemaphoreTake(this->mutex, (TickType_t)100) != pdTRUE)
  {
    ESP_LOGE(TAG, "mutex socket nao liberado");
    return ESP_FAIL;
  }

  if (client_fd == 0)
  {
    ESP_LOGE(TAG, "client fd is 0");

    xSemaphoreGive(this->mutex);
    return ESP_ERR_NOT_FOUND;
  }
  // else if (this->size_tx != 0)
  // {
  //   ESP_LOGE(TAG, "client fd is 0");
  //   return ESP_ERR_INVALID_STATE;
  // }
  else if (size > SOCKET_TX_BUFFER_MAX)
  {
    ESP_LOGE(TAG, "send data greater than %d", SOCKET_TX_BUFFER_MAX);

    xSemaphoreGive(this->mutex);
    return ESP_ERR_INVALID_ARG;
  }

  memcpy(this->buffer_tx, buffer, size);
  this->size_tx = size;

  err = send(client_fd, this->buffer_tx, this->size_tx, 0);

  if (err < 0)
  {
    ESP_LOGE(TAG, "error (%d) on send", errno);
    perror("send");
  }

  this->size_tx = 0;

  xSemaphoreGive(this->mutex);
  return err;
}
