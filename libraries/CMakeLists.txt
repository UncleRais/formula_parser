cmake_minimum_required(VERSION 3.16)

project(parser)

set(INCLUDES $ENV{CPATH})
if (NOT DEFINED NO_CONAN)
    set(INCLUDES ${CONAN_INCLUDE_DIRS_EIGEN})
endif()

add_library(parser_lib STATIC 
    parser.cpp
    utils.cpp
)

target_include_directories(parser_lib PUBLIC 
    "." 
    ${INCLUDES}
)