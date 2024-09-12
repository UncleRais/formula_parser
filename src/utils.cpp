#ifndef UTILS_CPP
#define UTILS_CPP

#include "utils.h"


void error(const str& str) {
	std::cerr << str << std::endl;
	exit(3);
}

str& delete_all(str& str, symbol symb) {
	str.erase(std::remove(str.begin(), str.end(), symb), str.end());
	return str;
}

size_type count_all(const str& s, symbol symb) {
	size_type count = 0;
	for (size_type position = s.find(symb); position != -1; position = s.find(symb, position + 1))
		count++;
	return count;
}

bool is_latin_symbol(symbol s) {
	return ('a' <= s && s <= 'z') || ('A' <= s && s <= 'Z');
};

bool is_latin_str(str s) {
	if (s.size() == 0 || s.size() > 1)
		error("Wrong format, latyn symbol contains of one symbol.");
	for (symbol& d : s)
		if (!is_latin_symbol(d)) return false;
	return true;
};

bool is_digit(symbol s) {
	return ('0' <= s && s <= '9');
};

bool is_number(str s) {
	size_type dots = count_all(s, '.');
	if (dots > 1)
		return false;
	for (symbol& d : s)
		if (!is_digit(d) && d != '.') return false;
	if (s.back() == '.') return false;
	return true;
};

template<>
double get_number(const str& number) {
	return std::stod(number);
}

template<>
float get_number(const str& number) {
	return std::stof(number);
}

template<>
long double get_number(const str& number) {
	return std::stold(number);
}

template<>
int get_number(const str& number) {
	return std::stoi(number);
}

template<>
size_type get_number(const str& number) {
	return std::stoul(number);
}


#endif
