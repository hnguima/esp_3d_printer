# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS
                        "ws_log.cpp"
                    INCLUDE_DIRS
                        "include"
                    PRIV_REQUIRES
                        http_server
                         )
