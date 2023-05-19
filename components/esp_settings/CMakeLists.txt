# # Menor versão com suporte a cross-compiling
cmake_minimum_required(VERSION 3.16)

idf_component_register(SRC_DIRS
                        "nanopb"
                        "generated"
                        "."
                    INCLUDE_DIRS   
                        "nanopb"
                        "generated"
                        "include"
                    REQUIRES 
                        # user libs
                        esp_filesystem
                         )
                        

add_custom_target(build_protos
                    COMMAND ${PYTHON} "${CMAKE_CURRENT_SOURCE_DIR}/script/build_protos.py" 
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(${COMPONENT_LIB} build_protos)