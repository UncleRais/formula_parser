cmake_minimum_required(VERSION 3.16)

project(parser_tests)

add_executable(unit_tests tests.cpp)
target_include_directories(unit_tests PUBLIC ".")
target_link_libraries(unit_tests
                      parser_lib
)

add_test(NAME unit_tests COMMAND unit_tests)

