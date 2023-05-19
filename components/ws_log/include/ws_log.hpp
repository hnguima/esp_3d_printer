#pragma once

#include <string>
#include <functional>

#include <http_server.hpp>

#define WEBSOCKET_LOG_BUFFER_SIZE 1024

#define WEBSOCKET_LOG_MAX_CLIENTS 2

class WebSocketLog
{
protected:
  static WebSocketLog *_singleton;
  WebSocketLog();

public:
  static void init();

  static WebSocketLog *get_instance();

  static esp_err_t uri_handler(httpd_req_t *req);
  static int vprintf(const char *str, va_list l);

  WebSocketLog(WebSocketLog &other) = delete;
  void operator=(const WebSocketLog &) = delete;

  static uint8_t buffer[WEBSOCKET_LOG_BUFFER_SIZE];

private:
  HTTPServer *server;

  static std::vector<int> clients;

  void send_async(uint8_t *, size_t);
};
