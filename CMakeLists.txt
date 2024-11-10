cmake_minimum_required(VERSION 3.16)

project(formula_parser)

set(CMAKE_CXX_STANDARD 20)

if(UNIX) 
    message(STATUS ">>> UNIX")
    set(CMAKE_CXX_COMPILER "/usr/bin/c++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O2 ")
endif()

if(MSVC) 
    message(STATUS ">>> MSVC")
    
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# unit tests
enable_testing()

if (NOT DEFINED NO_CONAN)
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        message(STATUS "Downloading conanbuildinfo.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS KEEP_RPATHS)
    else()
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    endif()
    conan_basic_setup()
endif()

add_subdirectory(libraries/parser)

add_subdirectory(src)

add_subdirectory(tests)