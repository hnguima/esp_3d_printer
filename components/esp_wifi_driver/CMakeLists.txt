# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS 
                        "esp_wifi_driver.cpp"
                    INCLUDE_DIRS   
                        "include"
                    REQUIRES 
                        esp_wifi
                        spiffs
                        nvs_flash
                        esp_system
                        esp_event
                        esp_hw_support
                        lwip
                         )
                        
                    