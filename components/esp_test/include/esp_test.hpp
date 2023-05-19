#pragma once

#include <string>
#include <functional>

#include <esp_err.h>

class GenericTest;
class TestManager
{

public:
  TestManager();
  TestManager(bool (*setup)(void));
  TestManager(bool (*setup)(void), bool (*cleanup)(void));

  void set_setup_func(bool (*setup)(void));
  void set_cleanup_func(bool (*cleanup)(void));

  void start();
  int run_all_tests();
  int run_test_by_index(int index);

  void print_test_menu_pt();
  int wait_for_input();

  int run_menu();

private:
  bool (*setup)(void);
  bool (*cleanup)(void);

  std::vector<GenericTest *> tests;
};
