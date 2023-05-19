# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS 
                        "esp_ethernet.cpp"
                    INCLUDE_DIRS   
                        "include"
                        "test"
                    REQUIRES 
                        esp_eth
                        esp_netif
                        esp_system
                        esp_event
                        lwip

                        # user libs
                        esp_test
                         )
                        
                    