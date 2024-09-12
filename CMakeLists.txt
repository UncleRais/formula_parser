cmake_minimum_required(VERSION 3.16)

project(formula_parser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "/usr/bin/c++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O2 ")

set(CMAKE_BUILD_TYPE Debug)

include_directories(./library/)

set(SRC ./src/utils.cpp 
		./src/parser.cpp 
		./src/main.cpp)	

set(INCLUDE ./library/utils.h 
		 	./library/parser.h 
		 	./library/expression.cpp)	


add_library(parser_lib ${SRC})
target_include_directories(parser_lib INTERFACE ${INCLUDE})

add_executable(PROJ ${SRC})

# unit tests
enable_testing()
add_subdirectory(tests)