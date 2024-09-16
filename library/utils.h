#pragma once

#define _USE_MATH_DEFINES
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <algorithm>
#include <math.h>

#include <typeinfo>
#include <typeindex>
#include <type_traits>

template <typename T> 
concept arithmetic = std::integral<T> || std::floating_point<T>;

void error(const std::string& str);

std::string& delete_all(std::string& str, char symb);
std::size_t count_all(const std::string& str, char symb);

bool is_latin_symbol(char s);
bool is_latin_str(std::string s);
bool is_digit(char s);
bool is_number(std::string s);

template<typename T>
T get_number(const std::string& number);