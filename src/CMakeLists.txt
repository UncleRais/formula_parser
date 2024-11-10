cmake_minimum_required(VERSION 3.16)

project(parser)

add_executable(Parser 
    main.cpp
)

target_include_directories(Parser PUBLIC 
    "."
)

target_link_libraries(Parser
    parser_lib
)