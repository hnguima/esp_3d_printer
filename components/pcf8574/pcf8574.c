#include "pcf8574.h"

uint8_t pcf_ports[3];

i2c_port_t i2c_port;

esp_err_t pcf_set_mask(uint8_t chip_addr, uint8_t pin_mask)
{

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, chip_addr << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, pin_mask, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 10 / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t pcf_set(uint8_t chip_addr, uint8_t pin, uint8_t value)
{
    uint8_t pin_mask = (pcf_ports[chip_addr - 0x20] & ~(1 << pin)) | (value << pin);
    pcf_ports[chip_addr - 0x20] = pin_mask;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, chip_addr << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, pin_mask, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 10 / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t pcf_get(uint8_t chip_addr, uint8_t *pin_mask)
{

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, chip_addr << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, pin_mask, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 10 / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t pcf_init(i2c_port_t port)
{
    i2c_port = port;

    pcf_get(0x20, &pcf_ports[0]);
    pcf_get(0x21, &pcf_ports[1]);
    pcf_get(0x22, &pcf_ports[2]);

    pcf_set_mask(0x20, 0x00);
    pcf_set_mask(0x21, 0x00);
    pcf_set_mask(0x22, 0x00);

    ESP_LOG_BUFFER_HEX("pcf", pcf_ports, 3);

    return ESP_OK;
}
