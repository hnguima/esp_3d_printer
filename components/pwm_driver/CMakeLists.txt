# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS
                        "pwm.c"
                    INCLUDE_DIRS
                        "include"
                         )