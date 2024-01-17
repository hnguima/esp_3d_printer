# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRCS 
                       "esp_printer.cpp"
                    INCLUDE_DIRS   
                        "include"
                    REQUIRES 
                        stepper_driver
                        pid_controller
                        pwm_driver
                        gcode_interpreter
                        pcf8574
                        thermistor
                        driver
                         )
                        
                    