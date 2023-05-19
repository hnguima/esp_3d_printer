#pragma once

#include <string>

#include <esp_err.h>
#include <esp_wifi.h>

#define WIFI_DRIVER_DEFAULT_SSID "ESP32_WIFI"
#define WIFI_DRIVER_DEFAULT_PASSWORD "12345678"
#define WIFI_DRIVER_DEFAULT_IP "192.168.0.1"

class WifiDriver
{
protected:
  static WifiDriver *_singleton;
  WifiDriver();

public:
  std::string ap_ssid;
  std::string ap_password;
  std::string ip;

  std::string sta_ssid;
  std::string sta_password;

  wifi_mode_t mode;

  esp_netif_t *ap_netif;
  esp_netif_t *sta_netif;

  esp_err_t init_AP();
  esp_err_t init_AP(std::string ip);
  esp_err_t init_AP(std::string ssid, std::string password);
  esp_err_t init_AP(std::string ssid, std::string password, std::string ip);

  esp_err_t init_STA(std::string ssid, std::string password);

  esp_err_t deinit();

  static WifiDriver *get_instance();

  WifiDriver(WifiDriver &other) = delete;
  void operator=(const WifiDriver &) = delete;

private:
  esp_err_t *setup();
};
