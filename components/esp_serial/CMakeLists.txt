# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS
                        "esp_serial.cpp"
                    INCLUDE_DIRS
                        "include"
                        "test"
                    REQUIRES
                        driver


                        # user libs
                        esp_test

                         )
