#ifndef __SDCARD___H
#define __SDCARD___H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sdcard_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __SDCARD___H */