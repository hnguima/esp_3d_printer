/* Example test application for testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "unity.h"

#define PRINT_BANNER() printf("\
\n____________________________________________________________\n")

void app_main(void)
{
  int test_count = unity_get_test_count();

  UNITY_BEGIN();

  // unity_run_all_tests();
  for (int i = 0; i < test_count; i++)
  {
    test_desc_t test_info;
    unity_get_test_info(i, &test_info);

    PRINT_BANNER();
    unity_run_test_by_index(i);
  }

  UNITY_END();
}