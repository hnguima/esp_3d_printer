#include <esp_test.hpp>
#include <esp_generic_test.hpp>

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "unity.h"

static const char *TAG = "test";

TestManager::TestManager() : TestManager(NULL, NULL) {}

TestManager::TestManager(bool (*setup)(void)) : TestManager(setup, NULL) {}

TestManager::TestManager(bool (*setup)(void), bool (*cleanup)(void)) : setup(setup), cleanup(cleanup) {}

void TestManager::set_setup_func(bool (*setup)(void))
{
  this->setup = setup;
}

void TestManager::set_cleanup_func(bool (*cleanup)(void))
{
  this->cleanup = cleanup;
}

void TestManager::start()
{

  if (this->setup && (this->setup() == false))
  {
    ESP_LOGI(TAG, "Setup returned false, not running tests");
    return;
  }

  this->run_menu();

  if (this->cleanup && (this->cleanup() == false))
  {
    ESP_LOGE(TAG, "Error while doing cleanup routine");
  }
}

int TestManager::run_all_tests()
{

  UNITY_BEGIN();

  for (GenericTest *test : GenericTest::tests)
  {

    test->run();
  }

  return UNITY_END();
}

int TestManager::run_test_by_index(int index)
{
  UNITY_BEGIN();

  Unity.TestFile = GenericTest::tests.at(index - 1)->file_name;
  GenericTest::tests.at(index - 1)->run();

  return UNITY_END();
}

void TestManager::print_test_menu_pt()
{
  printf("\n\nEscolha um teste para fazer:\n");

  uint8_t i = 1;
  for (GenericTest *test : GenericTest::tests)
  {
    printf("(%d)\t%s\n", i++, test->name);
  }

  printf("\nDigite o número do teste ou aperte ENTER para testar todos.\nDigite 'q' para sair\n");
}

int TestManager::wait_for_input()
{
  char resp[16];
  get_string(resp, 16);

  return atoi(resp);
}

int TestManager::run_menu()
{

  if (this->setup && (this->setup() == false))
  {
    ESP_LOGI(TAG, "Setup returned false, not running tests");
    return -1;
  }

  int test_result = 0;

  while (true)
  {
    this->print_test_menu_pt();

    char input[16];
    get_string(input, 16);

    // return atoi(input);

    if (strcmp(input, "q\n") == 0)
    {
      break;
    }

    if (strcmp(input, "\n") == 0 || atoi(input) > GenericTest::tests.size())
    {
      test_result = this->run_all_tests();
      continue;
    }

    test_result = this->run_test_by_index(atoi(input));
  }

  if (this->cleanup && (this->cleanup() == false))
  {
    ESP_LOGE(TAG, "Error while doing cleanup routine");
  }

  return test_result;
}
