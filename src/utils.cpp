#include "utils.h"


void error(const std::string& str) {
	std::cerr << str << std::endl;
	exit(3);
}

std::string& delete_all(std::string& str, char symb) {
	str.erase(std::remove(str.begin(), str.end(), symb), str.end());
	return str;
}

std::size_t count_all(const std::string& s, char symb) {
	std::size_t count = 0;
	for (std::size_t position = s.find(symb); position != -1; position = s.find(symb, position + 1))
		count++;
	return count;
}

bool is_latin_symbol(char s) {
	return ('a' <= s && s <= 'z') || ('A' <= s && s <= 'Z');
};

bool is_latin_str(std::string s) {
	if (s.size() == 0 || s.size() > 1)
		error("Wrong format, latyn symbol contains of one symbol.");
	for (char& d : s)
		if (!is_latin_symbol(d)) return false;
	return true;
};

bool is_digit(char s) {
	return ('0' <= s && s <= '9');
};

bool is_number(std::string s) {
	std::size_t dots = count_all(s, '.');
	if (dots > 1)
		return false;
	for (char& d : s)
		if (!is_digit(d) && d != '.') return false;
	if (s.back() == '.') return false;
	return true;
};

template<>
double get_number(const std::string& number) {
	return std::stod(number);
}

template<>
float get_number(const std::string& number) {
	return std::stof(number);
}

template<>
long double get_number(const std::string& number) {
	return std::stold(number);
}

template<>
int get_number(const std::string& number) {
	return std::stoi(number);
}

template<>
std::size_t get_number(const std::string& number) {
	return std::stoul(number);
}
