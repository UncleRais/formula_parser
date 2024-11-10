#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <functional>
#include <numeric>
#include <math.h>
#include <stdexcept>

#include <typeinfo>
#include <typeindex>
#include <type_traits>

template <typename T> 
concept arithmetic = std::integral<T> || std::floating_point<T>;
