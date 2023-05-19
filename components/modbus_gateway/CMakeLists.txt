# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS
                        "modbus_gateway.cpp"
                    INCLUDE_DIRS
                        "include"
                    REQUIRES
                        esp_timer

                        # user libs
                        esp_serial
                        esp_socket
                         )
