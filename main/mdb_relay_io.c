#include "mdb_relay_io.h"

#include <esp_log.h>

#include <driver/gpio.h>

static const char *TAG = "mdb_relay_io";

esp_err_t mdb_relay_io_init()
{
  // Saidas digitais
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  io_conf.mode = GPIO_MODE_OUTPUT;

  esp_err_t err = gpio_config(&io_conf);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "gpio_config error: %s", esp_err_to_name(err));
    return err;
  }

  err = gpio_set_level((gpio_num_t)GPIO_OUTPUT_ENABLE_3V_PIN, 0);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "error setting io level: %s", esp_err_to_name(err));
    return err;
  }

  return ESP_OK;
}
