#include <ws_log.hpp>

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "test";
std::vector<int> WebSocketLog::clients;

WebSocketLog *WebSocketLog::_singleton;
uint8_t WebSocketLog::buffer[WEBSOCKET_LOG_BUFFER_SIZE] = {0};

WebSocketLog::WebSocketLog() {}

void WebSocketLog::init()
{
  if (_singleton == nullptr)
  {
    _singleton = new WebSocketLog();
  }

  _singleton->server = HTTPServer::get_instance();

  httpd_uri_t ws_uri = {};
  ws_uri.uri = "/ws_log";
  ws_uri.method = HTTP_GET;
  ws_uri.handler = WebSocketLog::uri_handler;
  ws_uri.user_ctx = _singleton;
  ws_uri.is_websocket = true;

  _singleton->server->register_uri(&ws_uri);

  esp_log_set_vprintf(WebSocketLog::vprintf);

  // return 0;
}

esp_err_t WebSocketLog::uri_handler(httpd_req_t *req)
{
  WebSocketLog *ws_log = (WebSocketLog *)req->user_ctx;

  // uint8_t buf[128] = {0};
  // httpd_ws_frame_t ws_pkt;
  // memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
  // ws_pkt.payload = buf;
  // ws_pkt.type = HTTPD_WS_TYPE_TEXT;

  // esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 128);
  // if (ret != ESP_OK)
  // {
  //   ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
  //   return ret;
  // }

  for (int client : ws_log->clients)
  {
    if (client == httpd_req_to_sockfd(req))
    {
      return ESP_OK;
    }
  }

  if (ws_log->clients.size() < WEBSOCKET_LOG_MAX_CLIENTS)
  {
    ws_log->clients.push_back(httpd_req_to_sockfd(req));
  }

  return ESP_OK;
}

void WebSocketLog::send_async(uint8_t *buffer, size_t buffer_length)
{
  // esp_err_t err = ESP_OK;

  httpd_ws_frame_t ws_packet = {};
  ws_packet.type = HTTPD_WS_TYPE_TEXT;

  ws_packet.payload =  buffer;
  ws_packet.len = buffer_length;

  // printf("clients: %d\n", _singleton->clients.size());

  for (uint8_t i = 0; i < _singleton->clients.size(); i++)
  {
    if (httpd_ws_get_fd_info(_singleton->server->get_handle(), _singleton->clients.at(i)) != HTTPD_WS_CLIENT_WEBSOCKET)
    {
      printf("client %d is out\n", _singleton->clients.at(i));
      _singleton->clients.erase(_singleton->clients.begin() + i);
      continue;
    }

    esp_err_t err = httpd_ws_send_data_async(_singleton->server->get_handle(), _singleton->clients.at(i), &ws_packet, NULL, NULL);

    if (err != ESP_OK)
    {
      printf("error (%s) in socket send", esp_err_to_name(err));
      _singleton->clients.erase(_singleton->clients.begin() + i);
    }
  }

  // return err;
}

int WebSocketLog::vprintf(const char *str, va_list l)
{
  size_t buffer_length = 0;
  char task_name[16] = {0};
  memcpy(task_name, pcTaskGetName(xTaskGetCurrentTaskHandle()), 16);

  if (strncmp(task_name, "tiT", 16) != 0)
  {
    // memset(buffer, 0, WEBSOCKET_LOG_BUFFER_SIZE);
    buffer_length = vsprintf((char *)buffer, str, l);
    buffer[buffer_length] = '\0';

    printf((char *)buffer);
    _singleton->send_async(buffer, buffer_length);
  }

  return buffer_length;
}

// int ws_log_vprintf(const char *str, va_list l)
// {
//   int err = 0;
//   int len;
//   char task_name[16];
//   char *cur_task = pcTaskGetTaskName(xTaskGetCurrentTaskHandle());
//   strncpy(task_name, cur_task, 16);
//   task_name[15] = 0;

//   if (strncmp(task_name, "tiT", 16) != 0)
//   {
//     memset(buf, 0, MAX_WS_LEN);
//     vsprintf(buf, str, l);

//     ws_async_message(buf);
//   }
//   return vprintf(str, l);
// }

WebSocketLog *WebSocketLog::get_instance()
{
  return _singleton;
}
