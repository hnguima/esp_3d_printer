#include "esp_ethernet.hpp"

#include <freertos/FreeRTOS.h>

#include <esp_log.h>
#include <esp_event.h>

// lwip
#include <lwip/dns.h>
#include <lwip/inet.h>
#include <lwip/err.h>
#include <lwip/ip4_addr.h>

// sntp
#include <esp_sntp.h>

static const char *TAG = "Ethernet";

Ethernet *Ethernet::_singleton = nullptr;

static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

Ethernet::Ethernet() {}
Ethernet::Ethernet(const char *ip, const char *gw, const char *mask) : _ip(ip), _gw(gw), _mask(mask) {}

Ethernet *Ethernet::get_instance()
{
  return _singleton;
}

void Ethernet::deinit()
{
  esp_eth_driver_uninstall(this->handle);
  if (this->phy != NULL)
  {
    this->phy->del(this->phy);
  }
  esp_netif_destroy(this->netif);
  esp_netif_deinit();
}

void Ethernet::init()
{
  Ethernet::init("", "", "");
}

void Ethernet::init(const char *ip, const char *gw, const char *mask)
{
  esp_err_t err = ESP_OK;

  if (_singleton == nullptr)
  {
    _singleton = new Ethernet(ip, gw, mask);
  }

  // Initialize TCP/IP network interface (should be called only once in application)
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_init());
  // Create default event loop that running in background
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_create_default());

  // Create new default instance of esp-netif for Ethernet
  esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
  _singleton->netif = esp_netif_new(&cfg);

  if (strstr(_singleton->_ip, "") == NULL)
  {
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcpc_stop(_singleton->netif));
    esp_netif_ip_info_t eth_ip_info;

    ip4_addr_set_u32(&eth_ip_info.ip, ipaddr_addr(_singleton->_ip));
    ip4_addr_set_u32(&eth_ip_info.gw, ipaddr_addr(_singleton->_gw));
    ip4_addr_set_u32(&eth_ip_info.netmask, ipaddr_addr(_singleton->_mask));

    esp_netif_set_ip_info(_singleton->netif, &eth_ip_info);
  }

  // Init MAC and PHY configs to default
  eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
  eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

  phy_config.phy_addr = CONFIG_ETH_PHY_ADDR;
  phy_config.reset_gpio_num = CONFIG_ETH_PHY_RST;
  eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();

  esp32_emac_config.smi_mdc_gpio_num = CONFIG_ETH_PHY_MDC;
  esp32_emac_config.smi_mdio_gpio_num = CONFIG_ETH_PHY_MDIO;
  esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
  _singleton->phy = esp_eth_phy_new_lan87xx(&phy_config);

  esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, _singleton->phy);
  _singleton->handle = NULL;
  err = esp_eth_driver_install(&config, &_singleton->handle);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "Error (%s) while installing eth driver", esp_err_to_name(err));
    _singleton = nullptr;
    return;
  }
  /* attach Ethernet driver to TCP/IP stack */
  err = esp_netif_attach(_singleton->netif, esp_eth_new_netif_glue(_singleton->handle));
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "Error (%s) while attaching netif", esp_err_to_name(err));
    _singleton = nullptr;
    return;
  }

  // Register user defined event handers
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

  err = esp_eth_start(_singleton->handle);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "Error (%s) while starting eth", esp_err_to_name(err));
    _singleton = nullptr;
    return;
  }
}

#ifdef CONFIG_ETH_DNS_ENABLE

static void eth_dns_configure(esp_netif_t *netif)
{
  // Set DNS servers for internet connection
  esp_netif_dns_info_t dns_info;
  dns_info.ip.type = IPADDR_TYPE_V4;

  IP_ADDR4(&dns_info.ip, 8, 8, 8, 8);
  esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
  IP_ADDR4(&dns_info.ip, 1, 1, 1, 1);
  esp_netif_set_dns_info(netif, ESP_NETIF_DNS_BACKUP, &dns_info);
}

#ifdef CONFIG_ETH_SNTP_ENABLE

static void eth_sntp_init(void)
{
  setenv("TZ", "BRST+3BRDT+2,M10.3.0,M2.3.0", 1);
  tzset();

  // yield
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
}

#endif // CONFIG_ETH_SNTP_ENABLE

#endif // CONFIG_ETH_DNS_ENABLE

// callbacks:

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
  // Se o IP foi recebido pelo ESP32, significa que o serviço dhcp_c (client) está habilitado por padrão
  ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
  const esp_netif_ip_info_t *ip_info = &event->ip_info;

  ESP_LOGI(TAG, "\nESP_LOGIIP Address"
                "\n~~~~~~~~~~~~~~~~~~~~~~~~"
                "\nETHIP:   %03d.%03d.%03d.%03d"
                "\nETHMASK: %03d.%03d.%03d.%03d"
                "\nETHGW:   %03d.%03d.%03d.%03d"
                "\n~~~~~~~~~~~~~~~~~~~~~~~~",
           IP2STR(&ip_info->ip), IP2STR(&ip_info->netmask), IP2STR(&ip_info->gw));

#ifdef CONFIG_ETH_DNS_ENABLE
  // configure DNS servers
  eth_dns_configure(event->esp_netif);

#ifdef CONFIG_ETH_SNTP_ENABLE
  // configure SNTP
  if (sntp_restart() == false)
  {
    eth_sntp_init();
  }
#endif // CONFIG_ETH_SNTP_ENABLE

#endif // CONFIG_ETH_DNS_ENABLE
}

// Event handler for IP_EVENT_ETH_GOT_IP
static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  uint8_t mac_addr[6] = {0};
  /* we can get the ethernet driver handle from event data */
  esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

  switch (event_id)
  {

  case ETHERNET_EVENT_START:

    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
    ESP_LOGI(TAG, "Ethernet Started");
    ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    break;

  case ETHERNET_EVENT_CONNECTED:

    ESP_LOGI(TAG, "Ethernet Link Up");

    break;

  case ETHERNET_EVENT_DISCONNECTED:

    ESP_LOGI(TAG, "Ethernet Link Down");
    break;

  case ETHERNET_EVENT_STOP:

    ESP_LOGI(TAG, "Ethernet Stopped");
    break;

  default:
    break;
  }
}