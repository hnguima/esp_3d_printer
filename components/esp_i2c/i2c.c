#include "i2c.h"

static i2c_port_t i2c_port;

void i2c_init(i2c_port_t port)
{
    i2c_port = port;

    i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0);

    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = I2C_SCL_PIN,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_FREQ};

    i2c_param_config(i2c_port, &i2c_config);
}