#pragma once

#include <string>
#include <concepts>

namespace parser::utils {

template <typename T> 
concept arithmetic = std::integral<T> || std::floating_point<T>;

std::string trim(std::string str);
std::string& delete_all(std::string& str, char symb);
std::size_t count_all(const std::string& s, char symb);

bool is_latin_str(const std::string& s);
bool is_number(const std::string& s);

template<arithmetic T>
T get_number(const std::string& number) {
    if constexpr (std::is_same_v<T, float>)       
        return std::stof(number);
    if constexpr (std::is_same_v<T, double>)      
        return std::stod(number);
    if constexpr (std::is_same_v<T, long double>) 
        return std::stold(number);
    if constexpr (std::is_same_v<T, int>)         
        return std::stoi(number);
    if constexpr (std::is_same_v<T, std::size_t>) 
        return std::stoul(number);
}

}
