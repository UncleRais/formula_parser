#ifndef UTILS_H
#define UTILS_H

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

using str = std::string;
using symbol = char;
using num_type = double;
using size_type = std::size_t;

template <typename T> 
concept arithmetic = std::integral<T> || std::floating_point<T>;

void error(const str& str);

str& delete_all(str& str, symbol symb);
size_type count_all(const str& str, symbol symb);

bool is_latin_symbol(symbol s);
bool is_latin_str(str s);
bool is_digit(symbol s);
bool is_number(str s);

template<typename T>
T get_number(const str& number);

#endif