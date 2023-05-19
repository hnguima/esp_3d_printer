# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS 
                        "socket_tcp.cpp"
                        "socket_client_tcp.cpp"
                        "socket_server_tcp.cpp"
                    INCLUDE_DIRS 
                        "include"
                    REQUIRES
                        lwip
                        esp_event
                        esp_netif
                        )
                    