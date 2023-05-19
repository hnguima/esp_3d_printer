# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS 
                        "backend/http_server.cpp"
                    INCLUDE_DIRS   
                        "backend/include"
                    REQUIRES 
                        esp_http_server
                        esp_event
                        esp_netif
                        mbedtls
                        
                        # user libs
                        esp_filesystem
                         )

add_custom_target(build_interface
                    COMMAND npm install && npm run esp
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

spiffs_create_partition_image(server "${PROJECT_DIR}/build/server" FLASH_IN_PROJECT DEPENDS build_interface)
