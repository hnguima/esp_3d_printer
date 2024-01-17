# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS
                        "adc.c"
                    INCLUDE_DIRS
                        "include"
                    REQUIRES
                        driver
                        esp_adc
                        # esp_adc_cal
                         )