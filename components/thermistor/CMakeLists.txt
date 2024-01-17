# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS
                        "thermistor.c"
                    INCLUDE_DIRS
                        "include"
                    REQUIRES
                        adc_driver
                         )