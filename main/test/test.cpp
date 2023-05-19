#include <test.hpp>
#include <esp_generic_test.hpp>

#include "test_ethernet.hpp"
#include "test_serial_rs485.hpp"

#include "mdb_relay_io.h"
#include "driver/gpio.h"

#include <esp_log.h>

static const char *TAG = "test_module";

// GenericTest test_1(
//     "test 1", __LINE__, __FILE__,
//     []()
//     {
//       ESP_LOGI("test1", "Testing test 1");
//     });

// GenericTest test_2(
//     "test 2", __LINE__, __FILE__,
//     []()
//     {
//       ESP_LOGI("test2", "Testing test 2");
//     });

void test_module_init()
{
  TestManager *test_manager = new TestManager(
      []() -> bool
      {
        esp_err_t err = mdb_relay_io_init();

        if (err != ESP_OK)
        {
          ESP_LOGI(TAG, "Couldn't init io while doing test setup");
          return false;
        }

        if (gpio_get_level(GPIO_INPUT_TEST_PIN) == 0)
        {
          ESP_LOGI(TAG, "Test pin not active. Not running tests");
          return false;
        }

        ESP_LOGI(TAG, "Setup done, initializing tests");
        return true;
      },
      []() -> bool
      {
        ESP_LOGI("test", "doing cleanup");
        return true;
      });

  test_manager->run_menu();
}
