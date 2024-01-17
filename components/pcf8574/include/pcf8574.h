#ifndef __PCF8547___H
#define __PCF8547___H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
// Drivers
#include "i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif


esp_err_t pcf_init(i2c_port_t port);
esp_err_t pcf_set(uint8_t chip_addr, uint8_t pin, uint8_t value);
esp_err_t pcf_set_mask(uint8_t chip_addr, uint8_t pin_mask);
esp_err_t pcf_get(uint8_t chip_addr, uint8_t *pin_mask);

#ifdef __cplusplus
}
#endif 

#endif /* __PCF8547___H */