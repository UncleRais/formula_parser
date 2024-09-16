#pragma once
#include "parser.h"
#include <cassert>

template <typename T, std::size_t TupSize, std::size_t M>
void check_elem(const T& x, size_t id, const std::array<double, M>& input, const std::array<double, TupSize>& ref) {
	const auto res = x(input);
	assert(std::abs(res - ref[id]) < 1e-15);
	//std::cout << res << " ";
};

template <typename TupleT, std::size_t... Is, std::size_t TupSize = std::tuple_size_v<TupleT>, std::size_t M>
void tuple_check(const TupleT& tp, std::index_sequence<Is...>, const std::array<double, M>& input, const std::array<double, TupSize>& ref) {
	(check_elem(std::get<Is>(tp), Is, input, ref), ...);
}

template <typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>, std::size_t M>
void check_result(const TupleT& tp, const std::array<double, M>& input, const std::array<double, TupSize>& ref) {
	tuple_check(tp, std::make_index_sequence<TupSize>{}, input, ref);
}

void test_arithmetics() {
	variable<0> x;
	variable<1> y;
	variable<2> z;
	variable<3> t;

	std::array <double, 4 > input;
	input[0] = 1; input[1] = 2; input[2] = 3; input[3] = -6;
	const auto expressions = std::make_tuple(x + y, x - y, -t, x / y, x * y,
											 -int_constant<7>(), x + int_constant<5>(), scalar<double>(8.97) + scalar<double>(0.03),
											 int_constant<0>() + scalar<float>(1) - x + (y - y) / (z - t) + x);
	std::array<double, 9> ref{ 3, -1, 6, 0.5, 2, -7, 6, 9, 1 };
	check_result(expressions, input, ref);
}

void test_functions() {
	variable<0> x;
	variable<1> y;
	variable<2> t;

	std::array <double, 3> input;
	input[0] = M_PI; input[1] = M_PI / 4; input[2] = -1;
	const auto expressions = std::make_tuple(sin(x), cos(x), tan(y), ctan(y), exp(int_constant<0>()), log(scalar<double>(M_E)), sign(x / y),
											 sqrt(x * y * int_constant<4>()), sqr(t), abs(t));
	std::array<double, 10> ref{ 0, -1, 1, 1, 1, 1, 1, M_PI, 1, 1 };
	check_result(expressions, input, ref);

	const auto pow_exp = pow(x, t);
	const auto res = pow_exp(input);
	assert(std::abs(res - 1. / M_PI) < 1e-15);
}

void test_polish_notation() {

	// operators
	auto test = MathParser<double>("x : -x");
	assert(test.get_polish_notation() == "x~" && test.get_n_vars() == 1);
	test = MathParser<double>("x : x + x - x / x * x");
	assert(test.get_polish_notation() == "xx+xx/x*-" && test.get_n_vars() == 1);
	test = MathParser<double>("x : sin(x)");
	assert(test.get_polish_notation() == "xsin" && test.get_n_vars() == 1);
	assert(std::abs(test({ M_PI / 4 }) - std::sin(M_PI / 4)) < 1e-15);
	test = MathParser<double>("x : cos(x)");
	assert(test.get_polish_notation() == "xcos" && test.get_n_vars() == 1);
	assert(std::abs(test({ M_PI / 4 }) - std::cos(M_PI / 4)) < 1e-15);
	test = MathParser<double>("x : tan(x)");
	assert(test.get_polish_notation() == "xtan" && test.get_n_vars() == 1);
	assert(std::abs(test({ M_PI / 4 }) - std::tan(M_PI / 4)) < 1e-15);
	test = MathParser<double>("x : atan(x)");
	assert(test.get_polish_notation() == "xatan" && test.get_n_vars() == 1);
	assert(std::abs(test({ M_PI / 4 }) - std::atan(M_PI / 4)) < 1e-15);
	test = MathParser<double>("x : abs(x)");
	assert(test.get_polish_notation() == "xabs" && test.get_n_vars() == 1);
	assert(std::abs(test({ -M_PI / 4 }) - std::abs(-M_PI / 4)) < 1e-15);
	test = MathParser<double>("x : sign(x)");
	assert(test.get_polish_notation() == "xsign" && test.get_n_vars() == 1);
	assert(std::abs(test({ -M_PI / 4 }) + 1) < 1e-15);
	test = MathParser<double>("x : sqr(x)");
	assert(test.get_polish_notation() == "xsqr" && test.get_n_vars() == 1);
	assert(std::abs(test({ std::sqrt(2) }) - 2) < 1e-15);
	test = MathParser<double>("x : sqrt(x)");
	assert(test.get_polish_notation() == "xsqrt" && test.get_n_vars() == 1);
	assert(std::abs(test({ 5 }) - std::sqrt(5)) < 1e-15);
	test = MathParser<double>("x : log(x)");
	assert(test.get_polish_notation() == "xlog" && test.get_n_vars() == 1);
	assert(std::abs(test({ M_E }) - 1) < 1e-15);
	test = MathParser<double>("x : x^(0.3415234)");
	assert(test.get_polish_notation() == "x0.3415234^" && test.get_n_vars() == 1);
	assert(std::abs(test({ M_E }) - std::pow(M_E, 0.3415234)) < 1e-15);
	test = MathParser<double>("x : 0.411313 + .5 - x");
	assert(test.get_polish_notation() == "0.411313.5+x-" && test.get_n_vars() == 1);
	assert(std::abs(test({ 0.411313 }) - .5) < 1e-15);

	// trivial literal arithmetics
	test = MathParser<double>("x : -5 + 56.23424 - .51241 / 0.4321 * 4 / 10");
	assert(test.get_polish_notation() == "5~56.23424+.512410.4321/4*10/-" && test.get_n_vars() == 1);
	assert(std::abs(test({ 100 }) - 50.7599) < 1e-5);

	// general routine, examples
	test = MathParser<double>("v1 v2 v3 p1 p2 p3 m : (p1^2 + p2^2 + p3^2) / (2 * m) + .5 * (v1^2 + v2^2 + v3^2) ");
	assert(test.get_polish_notation() == "p12^p22^+p32^+2m*/.5v12^v22^+v32^+*+" && test.get_n_vars() == 7);
	assert(std::abs(test({ 1, 1, 1, 2, 2, 2, 2 }) - 4.5) < 1e-8);

	test = MathParser<double>("x a b : (x - 1)^(a - 1) * (x + 1)^(b + 1)");
	assert(test.get_polish_notation() == "x1-a1-^x1+b1+^*");
	assert(test.get_n_vars() == 3);
	assert(std::abs(test({ 1., 1., 1. }) - 4) < 1e-15);
	assert(std::abs(test({ 1., 100., 100. })) < 1e-15);
	assert(std::abs(test({ -1., 100., 100. })) < 1e-15);
	assert(std::abs(test({ 3., 3., 2. }) - 256) < 1e-15);

	test = MathParser<double>("x y t : x * cos(y) / exp(t) + 10");
	assert(test.get_polish_notation() == "xycos*texp/10+");
	assert(test.get_n_vars() == 3);
	assert(std::abs(test({ 2., M_PI, 10. }) - 9.99991) < 1e-5);
}
