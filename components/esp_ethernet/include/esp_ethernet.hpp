#pragma once

#include <string>

#include <esp_err.h>

#include <esp_eth.h>
#include <esp_netif.h>

class Ethernet
{
protected:
  static Ethernet *_singleton;
  Ethernet();
  Ethernet(const char *ip, const char *gw, const char *mask);

public:
  static void init();
  static void init(const char *ip, const char *gw, const char *mask);
  void deinit();

  static Ethernet *get_instance();

  Ethernet(Ethernet &other) = delete;
  void operator=(const Ethernet &) = delete;

private:
  const char *_ip;
  const char *_gw;
  const char *_mask;

  esp_eth_handle_t handle;
  esp_eth_phy_t *phy;
  esp_netif_t *netif;
};
