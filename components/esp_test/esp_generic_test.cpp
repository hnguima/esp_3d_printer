#include <esp_generic_test.hpp>

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "test";

std::vector<GenericTest *> GenericTest::tests;

void get_string(char *str, uint32_t size)
{
  uint8_t resp_index = 0;
  uint8_t ch;

  do
  {
    ch = getchar();
    if (ch == 255)
    {
      vTaskDelay(10 / portTICK_PERIOD_MS);
      continue;
    }

    if (ch == '\b')
    {
      if (resp_index != 0)
      {
        printf("\b \b");
        str[--resp_index] = 0;
      }
      continue;
    }

    printf("%c", ch);
    str[resp_index++] = ch;

    if (resp_index > size || ch == '\n')
    {
      str[resp_index++] = '\0';
      break;
    }
  } while (true);

  printf("\n");
}

GenericTest::GenericTest(const char *name, int line, const char *file_name, void (*function)(void)) : name(name),
                                                                                                      file_name(file_name),
                                                                                                      line(line),
                                                                                                      function(function)
{

  tests.push_back(this);
}

void GenericTest::run()
{
  Unity.CurrentTestName = this->name;
  Unity.CurrentTestLineNumber = (UNITY_LINE_TYPE)this->line;
  Unity.NumberOfTests++;
  UNITY_CLR_DETAILS();
  if (TEST_PROTECT())
  {
    setUp();
    this->function();
  }
  if (TEST_PROTECT())
  {
    tearDown();
  }
  UnityConcludeTest();
}
