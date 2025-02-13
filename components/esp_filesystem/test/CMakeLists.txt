idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "."
                       REQUIRES unity cmock esp_filesystem)


littlefs_create_partition_image(test_data "./test_data" FLASH_IN_PROJECT)
