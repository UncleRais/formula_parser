#pragma once

#include "utils_fwd.hpp"

void error(const std::string& str) {
	throw std::domain_error{str};
}

std::string& delete_all(std::string& str, char symb) {
	str.erase(std::remove(str.begin(), str.end(), symb), str.end());
	return str;
}

std::size_t count_all(const std::string& s, char symb) {
	return std::accumulate(s.begin(), s.end(), std::size_t(0), [](char a, char b){ return a == b ? 1 : 0; });
}

bool is_latin_str(std::string s) {
	if (s.size() == 0 || s.size() > 1)
		error("Wrong format, latyn symbol contains of one symbol.");
	for (char& d : s)
		if (!std::isalpha(d)) return false;
	return true;
};

bool is_number(std::string s) {
	std::size_t dots = count_all(s, '.');
	if (dots > 1)
		return false;
	for (char& d : s)
		if (!std::isdigit(d) && d != '.') return false;
	if (s.back() == '.') return false;
	return true;
};

template<typename T>
T get_number(const std::string& number) {
	if constexpr (std::is_same_v<T, float>)       return std::stof(number);
	if constexpr (std::is_same_v<T, double>)      return std::stod(number);
	if constexpr (std::is_same_v<T, long double>) return std::stold(number);
	if constexpr (std::is_same_v<T, int>)         return std::stoi(number);
	if constexpr (std::is_same_v<T, std::size_t>) return std::stoul(number);
}
