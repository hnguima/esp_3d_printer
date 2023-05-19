
#ifndef __MDB_RELAY_IO__
#define __MDB_RELAY_IO__

#include <esp_err.h>

#define GPIO_OUTPUT_ENABLE_3V_PIN GPIO_NUM_0
#define GPIO_INPUT_TEST_PIN GPIO_NUM_35

#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_OUTPUT_ENABLE_3V_PIN) || \
                             (1ULL << GPIO_INPUT_TEST_PIN))

#ifdef __cplusplus
extern "C"
{
#endif

  esp_err_t mdb_relay_io_init();

#ifdef __cplusplus
}
#endif

#endif // __MDB_RELAY_IO__
