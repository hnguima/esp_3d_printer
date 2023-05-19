#include "firmware_update.hpp"

#include <iostream>
#include <iomanip>

#include <freertos/FreeRTOS.h>

#include <esp_log.h>

#include <esp_partition.h>
#include <esp_ota_ops.h>

#include <http_server.hpp>

#include "mbedtls/sha256.h"

static const char *TAG = "FirmwareUpdate";

#define FIRMWARE_UPDATE_CHUNK_SIZE 4086

esp_err_t fw_update_uri_handler(httpd_req_t *req);

httpd_uri_t fw_update_uri = {
    .uri = "/update_firmware",
    .method = HTTP_PUT,
    .handler = fw_update_uri_handler,
    .user_ctx = NULL,
};

void print_progress_bar(double current, double total)
{
  const int barWidth = 50;
  int progress = static_cast<int>((current / total) * barWidth);
  std::cout << "\r\033[A\033[2K[";

  for (int i = 0; i < barWidth; ++i)
  {

    if (i == 23)
    {
      printf(" %3d %% ", static_cast<int>((current / total) * 100.0));
      i += 7;
    }
    else if (i < progress)
    {
      std::cout << "=";
    }
    else if (i == progress)
    {
      std::cout << ">";
    }
    else
    {
      std::cout << " ";
    }
  }

  std::cout << "] ( " << static_cast<int>((current / 1000)) << "KB / " << static_cast<int>((total / 1000)) << "KB )\r\n";
}

esp_err_t fw_update_uri_handler(httpd_req_t *req)
{
  esp_err_t err = ESP_OK;

  // TODO: check for some sort of authentication

  esp_ota_handle_t ota_handle;
  const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);

  mbedtls_sha256_context sha256_ctx;
  mbedtls_sha256_init(&sha256_ctx);

  uint32_t fw_size = req->content_len;
  if (fw_size == 0)
  {
    // TODO: take care of this case, return some kind of error
    return ESP_FAIL;
  }

  uint32_t recv_size = 0;
  uint8_t *fw_chunk = (uint8_t *)malloc(FIRMWARE_UPDATE_CHUNK_SIZE);

  mbedtls_sha256_starts(&sha256_ctx, false);

  for (uint32_t i = 0; recv_size < fw_size; i++)
  {
    int32_t fw_chunk_size = httpd_req_recv(req, (char *)fw_chunk, FIRMWARE_UPDATE_CHUNK_SIZE);

    if (fw_chunk_size == HTTPD_SOCK_ERR_TIMEOUT)
    {
      ESP_LOGW(TAG, "Socket Timeout");
      continue;
    }

    if (fw_chunk_size < 0)
    {
      ESP_LOGW(TAG, "OTA Error: %s", esp_err_to_name(fw_chunk_size));
      // TODO: take care of this case, return some kind of error
      return ESP_FAIL;
    }

    if (recv_size == 0)
    {
      err = esp_ota_begin(ota_partition, fw_size, &ota_handle);

      if (err != ESP_OK)
      {
        ESP_LOGI(TAG, "Error(%s) starting OTA update. Cancelling update.", esp_err_to_name(err));
        // TODO: take care of this case, return some kind of error to http
        return err;
      }

      ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%lx", ota_partition->subtype, ota_partition->address);
    }

    esp_ota_write(ota_handle, fw_chunk, fw_chunk_size);
    mbedtls_sha256_update(&sha256_ctx, fw_chunk, fw_chunk_size);

    recv_size += fw_chunk_size;

    print_progress_bar((double)recv_size, (double)fw_size);
  }

  ESP_LOGI(TAG, "Transfer completed: ( %d / %d )\r", (int)recv_size, (int)fw_size);

  // get calculated hash value
  uint8_t hash[32] = {0};
  mbedtls_sha256_finish(&sha256_ctx, hash);

  // get hash value received from request
  uint8_t recv_hash_size = httpd_req_get_hdr_value_len(req, "Hash") + 1;
  char *recv_hash = (char *)malloc(sizeof(char) * recv_hash_size);

  err = httpd_req_get_hdr_value_str(req, "Hash", recv_hash, recv_hash_size);
  if (err != ESP_OK)
  {
    ESP_LOGI(TAG, "Couldn't get hash from downloaded firmware (%s)", esp_err_to_name(err));
    // TODO: take care of this case, return some kind of error to http
    return err;
  }

  // verify if the hashes match
  char hash_str[64];
  for (size_t i = 0; i < 32; i++)
  {
    sprintf(hash_str + (i*2), "%02x", hash[i]);
  }

  if (!(strncmp(hash_str, recv_hash, 64) == 0))
  {
    ESP_LOGI(TAG, "SHA265 hashes don't match!");
    return ESP_FAIL;
  }

  const esp_app_desc_t *app_desc = esp_app_get_description();
  esp_app_desc_t update_desc;

  err = esp_ota_get_partition_description(ota_partition, &update_desc);
  if (err != ESP_OK)
  {
    ESP_LOGI(TAG, "OTA update error (%s)", esp_err_to_name(err));
    // TODO: take care of this case, return some kind of error to http
    return ESP_FAIL;
  }

  if (strcmp(app_desc->project_name, update_desc.project_name) != 0)
  {
    ESP_LOGI(TAG, "Firmware inválido");
    // TODO: take care of this case, return some kind of error to http
    return ESP_FAIL;
  }

  err = esp_ota_end(ota_handle);
  if (err != ESP_OK)
  {
    ESP_LOGI(TAG, "OTA update error (%s)", esp_err_to_name(err));
    // TODO: take care of this case, return some kind of error to http
    return ESP_FAIL;
  }

  // Lets update the partition
  err = esp_ota_set_boot_partition(ota_partition);
  if (err != ESP_OK)
  {
    ESP_LOGI(TAG, "OTA flash error (%s)", esp_err_to_name(err));
    // TODO: take care of this case, return some kind of error to http
    return ESP_FAIL;
  }

  const esp_partition_t *boot_partition = esp_ota_get_boot_partition();

  ESP_LOGI(TAG, "Next boot partition subtype %d at offset 0x%x", (unsigned int)boot_partition->subtype, (unsigned int)boot_partition->address);
  ESP_LOGI(TAG, "Restarting system...");

  // end response
  const char resp[] = "Success";
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

  vTaskDelay(2000 / portTICK_PERIOD_MS);
  esp_restart();

  return ESP_OK;
}

FirmwareUpdate::FirmwareUpdate()
{
  HTTPServer *http_server = HTTPServer::get_instance();
  http_server->register_uri(&fw_update_uri);
}