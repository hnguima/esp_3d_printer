#pragma once

#include <vector>

#include <esp_http_server.h>

typedef enum
{
  HTTP_USER_TYPE_COMMON = 0,
  HTTP_USER_TYPE_ADMIN = 1,
  HTTP_USER_TYPE_DEVELOPER = 2
} http_user_type_t;

typedef struct
{
  const char *username;
  const char *password;

  http_user_type_t type;

} http_user_t;

class HTTPServer
{
protected:
  static HTTPServer *_singleton;

  HTTPServer();

public:
  static void init();

  static HTTPServer *get_instance();

  void add_user(const char *username, const char *password);
  void add_user(const char *username, const char *password, http_user_type_t type);

  esp_err_t register_uri(const httpd_uri_t *uri);

  esp_err_t on(const char *uri, httpd_method_t method, esp_err_t (*func)(httpd_req_t *));

  httpd_handle_t get_handle() const { return this->handle; }

  static esp_err_t send_file_chunked(httpd_req_t *req, const char *file_name);

  HTTPServer(HTTPServer &other) = delete;
  void operator=(const HTTPServer &) = delete;

private:
  httpd_handle_t handle;

  std::vector<http_user_t *> users;

  bool auth(httpd_req_t *req);

  static httpd_uri_t default_uri;
  static esp_err_t default_uri_handler(httpd_req_t *req);
};
