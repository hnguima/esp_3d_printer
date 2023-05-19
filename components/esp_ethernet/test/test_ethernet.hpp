#include <esp_ethernet.hpp>
#include <esp_generic_test.hpp>

#include <esp_log.h>

#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#include "ping/ping_sock.h"

uint8_t erro_rede = 0;
uint8_t ping_end = 0;

static void test_on_ping_success(esp_ping_handle_t hdl, void *args)
{
  uint8_t ttl;
  uint16_t seqno;
  uint32_t elapsed_time, recv_len;
  ip_addr_t target_addr;
  esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
  esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
  esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
  esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
  esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
  printf("%ld bytes from %s icmp_seq=%d ttl=%d time=%ld ms\n",
         recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);

  erro_rede++;
}

static void test_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
  uint16_t seqno;
  ip_addr_t target_addr;
  esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
  esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
  printf("From %s icmp_seq=%d timeout\n", inet_ntoa(target_addr.u_addr.ip4), seqno);
}

static void test_on_ping_end(esp_ping_handle_t hdl, void *args)
{
  uint32_t transmitted;
  uint32_t received;
  uint32_t total_time_ms;

  esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
  esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
  esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
  printf("%ld packets transmitted, %ld received, time %ldms\n", transmitted, received, total_time_ms);

  ping_end = 1;
}

void test_initialize_ping()
{
  esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
  IP_ADDR4(&ping_config.target_addr, 192, 168, 1, 254);

  /* set callback functions */
  esp_ping_callbacks_t cbs;
  cbs.on_ping_success = test_on_ping_success;
  cbs.on_ping_timeout = test_on_ping_timeout;
  cbs.on_ping_end = test_on_ping_end;

  esp_ping_handle_t ping;
  esp_ping_new_session(&ping_config, &cbs, &ping);
  esp_ping_start(ping);
}

GenericTest test_ethernet(
    "Ethernet Test", __LINE__, __FILE__,
    []()
    {
      char resp[32];
      uint8_t ret = 0;

      printf("\n\nIniciando teste de rede\n\n");

      QUESTION("O cabo de rede está conectado?", "\n", "Conecte o cabo de rede e reinicie este teste.\n", resp, ret);

      if (!ret)
      {
        TEST_ASSERT(false);
      }

      Ethernet::init("192.168.1.10", "192.168.1.254", "255.255.255.0");
      vTaskDelay(2000 / portTICK_PERIOD_MS);

      QUESTION("Recebeu a confirmação de conexão?", "Iniciando teste de ping:\n", "Verifique a conexão de rede.\n", resp, ret);

      test_initialize_ping();

      while (!ping_end)
      {
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }

      TEST_ASSERT_GREATER_OR_EQUAL(5, erro_rede);

      // ESP_LOGI("Ethernet Test", "Testing Ethernet");
    });