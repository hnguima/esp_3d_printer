# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS 
                        "stepper_driver.cpp"
                        "stepper_timer.cpp"
                    INCLUDE_DIRS   
                        "include"
                    REQUIRES 
                        driver
                         )
                        
                    