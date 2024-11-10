cmake_minimum_required(VERSION 3.16)

project(parser_tests)

if (NOT DEFINED NO_CONAN)
    set(INCLUDES ${CONAN_INCLUDE_DIRS_BOOST-EXT-UT})
endif()

add_subdirectory(parser_tests)

add_executable(unit_tests all_tests.cpp)
target_include_directories(unit_tests PUBLIC ".")
target_link_libraries(unit_tests
    parser_test_lib
)

add_test(NAME unit_tests COMMAND unit_tests)

