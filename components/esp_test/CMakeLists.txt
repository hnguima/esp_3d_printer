# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS
                        "esp_test.cpp"
                        "esp_generic_test.cpp"
                    INCLUDE_DIRS
                        "include"
                    REQUIRES
                        unity
                         )
