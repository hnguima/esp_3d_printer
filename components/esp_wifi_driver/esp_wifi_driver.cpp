#include "esp_wifi_driver.hpp"

#include <freertos/FreeRTOS.h>

#include <esp_log.h>
#include <esp_event.h>

#include <nvs_flash.h>

#include <lwip/err.h>
#include <lwip/ip4_addr.h>

#include <lwip/dns.h>
#include <lwip/inet.h>
#include <esp_sntp.h>

static const char *TAG = "WifiDriver";

WifiDriver *WifiDriver::_singleton = nullptr;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);

static void wifi_dns_configure(esp_netif_t *netif);
static void wifi_sntp_init(void);

WifiDriver::WifiDriver()
{

  esp_log_level_set("wifi", ESP_LOG_ERROR);
  esp_log_level_set("wifi_init", ESP_LOG_ERROR);

  nvs_flash_init();

  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_create_default());

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &wifi_event_handler,
                                                      NULL,
                                                      &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &wifi_event_handler,
                                                      NULL,
                                                      &instance_got_ip));

  this->ap_netif = esp_netif_create_default_wifi_ap();
  assert(this->ap_netif);
  this->sta_netif = esp_netif_create_default_wifi_sta();
  assert(this->sta_netif);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
}

WifiDriver *WifiDriver::get_instance()
{

  if (_singleton == nullptr)
  {
    _singleton = new WifiDriver();
  }
  return _singleton;
}

esp_err_t WifiDriver::init_AP()
{
  return this->init_AP(WIFI_DRIVER_DEFAULT_SSID,
                       WIFI_DRIVER_DEFAULT_PASSWORD,
                       WIFI_DRIVER_DEFAULT_IP);
}

esp_err_t WifiDriver::init_AP(std::string ip)

{
  return this->init_AP(WIFI_DRIVER_DEFAULT_SSID,
                       WIFI_DRIVER_DEFAULT_PASSWORD,
                       ip);
}

esp_err_t WifiDriver::init_AP(std::string ssid, std::string password)
{
  return this->init_AP(ssid,
                       password,
                       WIFI_DRIVER_DEFAULT_IP);
}

esp_err_t WifiDriver::init_AP(std::string ssid, std::string password, std::string ip)
{
  ESP_LOGI(TAG, "%s | %s at %s", ssid.c_str(), password.c_str(), ip.c_str());

  if (this->mode == WIFI_MODE_STA)
  {
    this->mode = WIFI_MODE_APSTA;
  }
  else
  {
    this->mode = WIFI_MODE_AP;
  }

  this->ap_ssid = ssid;
  this->ap_password = password;
  this->ip = ip;

  this->setup();

  return ESP_OK;
}

esp_err_t WifiDriver::init_STA(std::string ssid, std::string password)
{
  ESP_LOGI(TAG, "%s | %s", ssid.c_str(), password.c_str());

  if (this->mode == WIFI_MODE_AP)
  {
    this->mode = WIFI_MODE_APSTA;
  }
  else
  {
    this->mode = WIFI_MODE_STA;
  }

  this->sta_ssid = ssid;
  this->sta_password = password;

  this->setup();

  return ESP_OK;
}

esp_err_t *WifiDriver::setup()
{

  ESP_ERROR_CHECK(esp_wifi_set_mode(this->mode));

  // Wifi AP block:
  if ((this->mode == WIFI_MODE_AP) || (this->mode == WIFI_MODE_APSTA))
  {
    // Set configuration for the wifi driver
    wifi_config_t wifi_ap_config = {};
    wifi_ap_config.ap.ssid_len = (uint8_t)this->ap_ssid.length();
    wifi_ap_config.ap.channel = 6;        // todo: add a define
    wifi_ap_config.ap.max_connection = 5; // todo: add a define;
    wifi_ap_config.ap.authmode = this->ap_password.length() != 0 ? WIFI_AUTH_WPA_WPA2_PSK : WIFI_AUTH_OPEN;
    strcpy((char *)wifi_ap_config.ap.ssid, this->ap_ssid.c_str());
    strcpy((char *)wifi_ap_config.ap.password, this->ap_password.c_str());

    // Set ip configuration
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_stop(this->ap_netif));

    esp_netif_ip_info_t wifi_ip_info;
    ip4_addr_set_u32(&wifi_ip_info.ip, ipaddr_addr(this->ip.c_str()));
    ip4_addr_set_u32(&wifi_ip_info.gw, ipaddr_addr("192.168.0.254"));
    ip4_addr_set_u32(&wifi_ip_info.netmask, ipaddr_addr("255.255.255.0"));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_set_ip_info(this->ap_netif, &wifi_ip_info));

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_start(this->ap_netif));

    // Config Wifi
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             this->ap_ssid.c_str(), this->ap_password.c_str(), 6);
  }

  // Wifi STA block:
  if ((this->mode == WIFI_MODE_STA) || (this->mode == WIFI_MODE_APSTA))
  {
    wifi_config_t wifi_sta_config = {};
    strcpy((char *)wifi_sta_config.sta.ssid, this->sta_ssid.c_str());
    strcpy((char *)wifi_sta_config.sta.password, this->sta_password.c_str());

    // Config Wifi
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config));

    ESP_LOGI(TAG, "wifi_init_sta finished.");
  }

  ESP_ERROR_CHECK(esp_wifi_start());

  if (this->mode == WIFI_MODE_STA || this->mode == WIFI_MODE_APSTA)
  {
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_connect());
  }

  return ESP_OK;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
  if (event_id == WIFI_EVENT_AP_STACONNECTED)
  {
    ESP_LOGI(TAG, "A STA connected to AP");
  }
  else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
  {
    ESP_LOGI(TAG, "A STA disconected from AP");
  }

  if (event_base == WIFI_EVENT && (event_id == WIFI_EVENT_STA_START || event_id == WIFI_EVENT_STA_DISCONNECTED))
  {
    esp_wifi_connect();
    ESP_LOGI(TAG, "Retrying connection to AP...");
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

    wifi_dns_configure(event->esp_netif);

    if (sntp_restart() == false)
    {
      wifi_sntp_init();
    }
  }
}

static void wifi_dns_configure(esp_netif_t *netif)
{
  // Set DNS servers for internet connection
  esp_netif_dns_info_t dns_info;
  dns_info.ip.type = IPADDR_TYPE_V4;

  IP_ADDR4(&dns_info.ip, 8, 8, 8, 8);
  esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
  IP_ADDR4(&dns_info.ip, 1, 1, 1, 1);
  esp_netif_set_dns_info(netif, ESP_NETIF_DNS_BACKUP, &dns_info);
}

static void wifi_sntp_init(void)
{
  setenv("TZ", "BRST+3BRDT+2,M10.3.0,M2.3.0", 1);
  tzset();

  // yield
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
}
