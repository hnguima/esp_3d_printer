# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(cubic_3d_printer)

spiffs_create_partition_image(data "./data" FLASH_IN_PROJECT)
# spiffs_create_partition_image(server "./build/http_server" FLASH_IN_PROJECT DEPENDS compress_server)
