/* SD card and FAT filesystem example.
   This example uses SPI peripheral to communicate with SD card.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include <dirent.h>
#include <stdio.h>

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"

void sdcard_init(void)
{
    esp_err_t ret;

    gpio_pulldown_dis(27);
    gpio_pullup_en(27);

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 1,
        .allocation_unit_size = 16 * 1024};

    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz (range 400kHz - 20MHz for SDSPI)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    // host.max_freq_khz = 1000;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = 13,
        .miso_io_num = 34,
        .sclk_io_num = 14,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = 27;
    slot_config.host_id = host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    struct dirent *dp;
    DIR *dir = opendir("/sdcard");
    if (dir == NULL)
    {
        ESP_LOGE(TAG, "Can't Open Dir.");
    }
    while ((dp = readdir(dir)) != NULL)
    {
        ESP_LOGI(TAG, "[%s]", dp->d_name);
    }
    closedir(dir);

    // char line[128];
    // int len = 0;

    // FILE *fp = fopen("/sdcard/CUBE~1.GCO", "r");
    // if (fp == NULL)
    //     exit(EXIT_FAILURE);

    // while ((len = file_get_line(fp, line)) > 0)
    // {
    //     printf("%d: %s", len, line);
    // }

    // fclose(fp);

    // struct stat *file_stat;
    // stat("/sdcard/cube.gcode", file_stat);

    // // Use POSIX and C standard library functions to work with files.

    // // First create a file.
    // const char *file_hello = MOUNT_POINT "/hello.txt";
    // char data[EXAMPLE_MAX_CHAR_SIZE];
    // snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Hello", card->cid.name);
    // ret = s_example_write_file(file_hello, data);
    // if (ret != ESP_OK)
    // {
    //     return;
    // }

    // const char *file_foo = MOUNT_POINT "/foo.txt";

    // // Check if destination file exists before renaming
    // struct stat st;
    // if (stat(file_foo, &st) == 0)
    // {
    //     // Delete it if it exists
    //     unlink(file_foo);
    // }

    // // Rename original file
    // ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
    // if (rename(file_hello, file_foo) != 0)
    // {
    //     ESP_LOGE(TAG, "Rename failed");
    //     return;
    // }

    // ret = s_example_read_file(file_foo);
    // if (ret != ESP_OK)
    // {
    //     return;
    // }

    // // Format FATFS
    // ret = esp_vfs_fat_sdcard_format(mount_point, card);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(ret));
    //     return;
    // }

    // if (stat(file_foo, &st) == 0)
    // {
    //     ESP_LOGI(TAG, "file still exists");
    //     return;
    // }
    // else
    // {
    //     ESP_LOGI(TAG, "file doesnt exist, format done");
    // }

    // const char *file_nihao = MOUNT_POINT "/nihao.txt";
    // memset(data, 0, EXAMPLE_MAX_CHAR_SIZE);
    // snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Nihao", card->cid.name);
    // ret = s_example_write_file(file_nihao, data);
    // if (ret != ESP_OK)
    // {
    //     return;
    // }

    // // Open file for reading
    // ret = s_example_read_file(file_nihao);
    // if (ret != ESP_OK)
    // {
    //     return;
    // }

    // // All done, unmount partition and disable SPI peripheral
    // esp_vfs_fat_sdcard_unmount(mount_point, card);
    // ESP_LOGI(TAG, "Card unmounted");

    // // deinitialize the bus after all devices are removed
    // spi_bus_free(host.slot);
}