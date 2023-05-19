#pragma once

#include <string>
#include <string.h>
#include <functional>

#include <unity.h>
#include <esp_err.h>

#define QUESTION(_qst, _msg_t, _msg_f, _str, _ret)                 \
  while (true)                                                     \
  {                                                                \
    printf("%s (S/N): \n", _qst);                                  \
    get_string(_str, 32);                                          \
    _ret = false;                                                  \
    if (strcmp("N\n", _str) == 0 || strcmp("n\n", _str) == 0)      \
    {                                                              \
      printf(_msg_f);                                              \
      break;                                                       \
    }                                                              \
    else if (strcmp("S\n", _str) == 0 || strcmp("s\n", _str) == 0) \
    {                                                              \
      printf(_msg_t);                                              \
      _ret = true;                                                 \
      break;                                                       \
    }                                                              \
    printf("Não reconhecido. Digite S ou N: ");                    \
  }

class GenericTest
{
public:
  GenericTest(const char *name, int line, const char *file_name, void (*function)(void));

  void run();

  static std::vector<GenericTest *> tests;
  const char *name;
  const char *file_name;

private:
  int line;
  void (*function)(void);
};

void get_string(char *str, uint32_t size);
