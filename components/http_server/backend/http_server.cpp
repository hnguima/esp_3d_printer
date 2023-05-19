#include "http_server.hpp"

#include <string>
#include "mbedtls/base64.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"

#include <esp_err.h>
#include <esp_log.h>

#include "esp_filesystem.hpp"

static const char *TAG = "HTTPServer";

HTTPServer *HTTPServer::_singleton = nullptr;

#define HTTP_MAX_BUFFER_SIZE 1024

esp_err_t HTTPServer::default_uri_handler(httpd_req_t *req)
{
  std::string uri(req->uri);

  HTTPServer *server = HTTPServer::get_instance();

  if (server->auth(req) == false)
  {
    httpd_resp_set_status(req, "401 UNAUTHORIZED");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"User interface\"");
    httpd_resp_send(req, NULL, 0);
    return ESP_FAIL;
  }

  ESP_LOGI(TAG, "Hello %s called method %d", uri.c_str(), req->method);

  // TODO: if uri = "/"  redirect to "/index"
  // TODO: search file in fs and proceed accordig to method

  std::string file_name = "/server";

  {
    // TODO: remember to move this to its own function
    if (uri == "/")
    {
      file_name += "/index.html";
    }
    else if (!(uri.find(".") != std::string::npos))
    {
      file_name += uri + ".html";
    }
    else
    {
      file_name += uri;
    }
  }

  {
    // TODO: remember to move this to its own function
    if (uri.find(".html") != std::string::npos)
    {
      httpd_resp_set_type(req, "text/html");
    }
    else if (uri.find(".js") != std::string::npos)
    {
      httpd_resp_set_type(req, "text/javascript");
    }
  }

  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

  return HTTPServer::send_file_chunked(req, file_name.c_str());
}

httpd_uri_t HTTPServer::default_uri = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = HTTPServer::default_uri_handler,
    .user_ctx = NULL,
};

HTTPServer::HTTPServer() {}

void HTTPServer::init()
{
  esp_err_t err = ESP_OK;

  if (_singleton == nullptr)
  {
    _singleton = new HTTPServer();
  }

  _singleton->handle = NULL;

  httpd_config_t http_config = HTTPD_DEFAULT_CONFIG();
  http_config.uri_match_fn = httpd_uri_match_wildcard;
  http_config.max_uri_handlers = 10;
  http_config.backlog_conn = 10;
  http_config.max_open_sockets = 3;
  http_config.lru_purge_enable = true;
  http_config.stack_size = 8192;

  err = ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_start(&_singleton->handle, &http_config));

  if (err)
  {
    ESP_LOGE(TAG, "Error(%s) starting http server...", esp_err_to_name(err));
    return;
  }

  err = ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_register_uri_handler(_singleton->handle, &default_uri));

  ESP_LOGI(TAG, "Server Started");
}

HTTPServer *HTTPServer::get_instance()
{
  return _singleton;
}

void HTTPServer::add_user(const char *username, const char *password)
{
  this->add_user(username, password, HTTP_USER_TYPE_COMMON);
  return;
}

void HTTPServer::add_user(const char *username, const char *password, http_user_type_t type)
{
  http_user_t *user = new http_user_t;

  user->username = username;
  user->password = password;
  user->type = type;

  this->users.push_back(user);
}

bool HTTPServer::auth(httpd_req_t *req)
{

  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "users size: %d", this->users.size());

  if (this->users.size() == 0)
  {
    return true;
  }

  size_t auth_header_size = httpd_req_get_hdr_value_len(req, "Authorization") + 1;

  if (!(auth_header_size > 1))
  {
    ESP_LOGE(TAG, "No auth header received");
    return false;
  }

  char *auth_header = (char *)calloc(auth_header_size, 1);

  err = httpd_req_get_hdr_value_str(req, "Authorization", auth_header, auth_header_size);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "No auth value received");
    free(auth_header);
    return false;
  }

  size_t auth_decoded_size;
  mbedtls_base64_decode(NULL, 0, &auth_decoded_size, (const unsigned char *)(auth_header + 6), strlen(auth_header + 6));

  char *auth_decoded = (char *)calloc(auth_decoded_size + 1, 1);
  mbedtls_base64_decode((unsigned char *)auth_decoded, auth_decoded_size + 1, &auth_decoded_size,
                        (const unsigned char *)(auth_header + 6), strlen(auth_header + 6));
  free(auth_header);

  int divider_pos = strchr(auth_decoded, ':') - auth_decoded;
  if (divider_pos <= 0)
  {
    free(auth_decoded);
    return false;
  }

  char *auth_user = (char *)calloc(divider_pos + 1, 1);
  memcpy(auth_user, auth_decoded, divider_pos);

  char *auth_pass = (char *)calloc(strlen(auth_decoded) - divider_pos, 1);
  memcpy(auth_pass, auth_decoded + divider_pos + 1, strlen(auth_decoded) - divider_pos - 1);

  free(auth_decoded);

  for (http_user_t *user : this->users)
  {
    // ESP_LOGI(TAG, "%s == %s?", user->username, auth_user);
    // ESP_LOGI(TAG, "%s == %s?", user->password, auth_pass);

    if ((strcmp(user->username, auth_user) == 0) &&
        (strcmp(user->password, auth_pass) == 0))
    {
      free(auth_user);
      free(auth_pass);
      return true;
    }
  }

  ESP_LOGE(TAG, "Not authenticated");
  free(auth_user);
  free(auth_pass);

  return false;
}

esp_err_t HTTPServer::register_uri(const httpd_uri_t *uri_handler)
{
  esp_err_t err = ESP_OK;

  err = ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_unregister_uri_handler(this->handle, "/*", uri_handler->method));
  err = ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_register_uri_handler(this->handle, uri_handler));

  default_uri.method = uri_handler->method;
  err = ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_register_uri_handler(this->handle, &default_uri));

  return err;
}

esp_err_t HTTPServer::on(const char *uri, httpd_method_t method, esp_err_t (*func)(httpd_req_t *))
{
  esp_err_t err = ESP_OK;

  httpd_uri_t new_uri = {
      .uri = uri,
      .method = method,
      .handler = func,
      .user_ctx = NULL,
  };

  err = ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_unregister_uri_handler(this->handle, "/*", new_uri.method));
  err = ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_register_uri_handler(this->handle, &new_uri));

  default_uri.method = new_uri.method;
  err = ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_register_uri_handler(this->handle, &default_uri));

  return err;
}

esp_err_t HTTPServer::send_file_chunked(httpd_req_t *req, const char *file_name)
{
  FileSystem *fs = FileSystem::get_instance();

  if (!fs->find(file_name))
  {
    httpd_resp_set_status(req, HTTPD_404);
    httpd_resp_send(req, NULL, 0);
  }

  int size_read = 0;
  char *file_buffer = (char *)calloc(HTTP_MAX_BUFFER_SIZE, 1);

  do
  {
    size_read = fs->read(file_name, file_buffer, HTTP_MAX_BUFFER_SIZE);
    httpd_resp_send_chunk(req, file_buffer, size_read);
  } while (size_read == HTTP_MAX_BUFFER_SIZE);

  free(file_buffer);

  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}
