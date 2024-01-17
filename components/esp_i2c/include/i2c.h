#ifndef __I2C___H
#define __I2C___H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
// Drivers
#include "driver/i2c.h"

#define I2C_SDA_PIN 33
#define I2C_SCL_PIN 32

#define I2C_FREQ 100000

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/


void i2c_init(i2c_port_t port);

#ifdef __cplusplus
}
#endif

#endif /* __I2C___H */