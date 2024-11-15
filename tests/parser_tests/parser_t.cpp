#include "parser.hpp"

#include <numbers>
#include <limits>

#include <boost/ut.hpp>

namespace {
using namespace boost::ut;
using namespace ex;
using namespace std::numbers;

template <typename T, std::size_t TupSize, std::size_t M>
void check_elem(const T& x, size_t id, const std::array<double, M>& input, const std::array<double, TupSize>& ref) {
	const auto res = x(input);
	const double val = std::abs(double(res) - ref[id]);
	expect(val < std::numeric_limits<double>::epsilon() * 3);
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

const suite<"parser"> _ = [] {

	"expression_arithmetics"_test = [] {
		variable<0> x;
		variable<1> y;
		variable<2> z;
		variable<3> t;

		constexpr std::array<double, 4> input {1, 2, 3, -6};
		const auto expressions = std::make_tuple(x + y, x - y, -t, x / y, x * y,
												-int_constant<7>(), x + int_constant<5>(), scalar<double>(8.97) + scalar<double>(0.03),
												int_constant<0>() + scalar<float>(1) - x + (y - y) / (z - t) + x);
		constexpr std::array<double, 9> ref{ 3, -1, 6, 0.5, 2, -7, 6, 9, 1 };
		check_result(expressions, input, ref);
	};

	"expression_functions"_test = [] {
		variable<0> x;
		variable<1> y;
		variable<2> t;

		constexpr std::array <double, 3> input{pi, pi / 4, -1};
		const auto expressions = std::make_tuple(sin(x), cos(x), tan(y), ctan(y), exp(int_constant<0>()), ex::log(scalar<double>(e)), sign(x / y),
												 sqrt(x * y * int_constant<4>()), sqr(t), abs(t));
		constexpr std::array<double, 10> ref{ 0, -1, 1, 1, 1, 1, 1, pi, 1, 1 };
		check_result(expressions, input, ref);

		const auto pow_exp = pow(x, t);
		const auto res = pow_exp(input);
		const double val = std::abs(double(res) - 1. / pi);
		expect(val < std::numeric_limits<double>::epsilon());
	};

	"polish_notation"_test = [] {
		auto test = MathParser<double>("x : -x");
		expect(test.get_polish_notation() == "x~" and test.get_n_vars() == 1);

		test = MathParser<double>("x : x + x - x / x * x");
		expect(test.get_polish_notation() == "xx+xx/x*-" and test.get_n_vars() == 1);

		test = MathParser<double>("x : sin(x)");
		expect(test.get_polish_notation() == "xsin" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ pi / 4 }) - std::sin(pi / 4)), std::numeric_limits<double>::epsilon()));

		test = MathParser<double>("x : cos(x)");
		expect(test.get_polish_notation() == "xcos" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ pi / 4 }) - std::cos(pi / 4)), std::numeric_limits<double>::epsilon()));

		test = MathParser<double>("x : tan(x)");
		expect(test.get_polish_notation() == "xtan" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ pi / 4 }) - std::tan(pi / 4)), std::numeric_limits<double>::epsilon()));

		test = MathParser<double>("x : atan(x)");
		expect(test.get_polish_notation() == "xatan" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ pi / 4 }) - std::atan(pi / 4)), std::numeric_limits<double>::epsilon()));

		test = MathParser<double>("x : abs(x)");
		expect(test.get_polish_notation() == "xabs" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ -pi / 4 }) - std::abs(-pi / 4)), std::numeric_limits<double>::epsilon()));

		test = MathParser<double>("x : sign(x)");
		expect(test.get_polish_notation() == "xsign" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ -pi / 4 }) + 1), std::numeric_limits<double>::epsilon()));

		test = MathParser<double>("x : sqr(x)");
		expect(test.get_polish_notation() == "xsqr" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ std::sqrt(2) }) - 2), std::numeric_limits<double>::epsilon() * 3));

		test = MathParser<double>("x : sqrt(x)");
		expect(test.get_polish_notation() == "xsqrt" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ 5 }) - std::sqrt(5)), std::numeric_limits<double>::epsilon() * 3));

		test = MathParser<double>("x : log(x)");
		expect(test.get_polish_notation() == "xlog" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ e }) - 1), std::numeric_limits<double>::epsilon()));

		test = MathParser<double>("x : x^(0.3415234)");
		expect(test.get_polish_notation() == "x0.3415234^" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ e }) - std::pow(e, 0.3415234)), std::numeric_limits<double>::epsilon()));

		test = MathParser<double>("x : 0.411313 + .5 - x");
		expect(test.get_polish_notation() == "0.411313.5+x-" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ 0.411313 }) - .5), std::numeric_limits<double>::epsilon()));

		// trivial literal arithmetics
		test = MathParser<double>("x : -5 + 56.23424 - .51241 / 0.4321 * 4 / 10");
		expect(test.get_polish_notation() == "5~56.23424+.512410.4321/4*10/-" and test.get_n_vars() == 1);
		expect(lt(std::abs(test({ 100 }) - 50.7599), 1e-5));

		// general routine, examples
		test = MathParser<double>("v1 v2 v3 p1 p2 p3 m : (p1^2 + p2^2 + p3^2) / (2 * m) + .5 * (v1^2 + v2^2 + v3^2) ");
		expect(test.get_polish_notation() == "p12^p22^+p32^+2m*/.5v12^v22^+v32^+*+" and test.get_n_vars() == 7);
		expect(lt(std::abs(test({ 1, 1, 1, 2, 2, 2, 2 }) - 4.5), 1e-8));

		test = MathParser<double>("x a b : (x - 1)^(a - 1) * (x + 1)^(b + 1)");
		expect(test.get_polish_notation() == "x1-a1-^x1+b1+^*");
		expect(test.get_n_vars() == 3);
		expect(lt(std::abs(test({ 1., 1., 1. }) - 4), std::numeric_limits<double>::epsilon()));
		expect(lt(std::abs(test({ 1., 100., 100. })), std::numeric_limits<double>::epsilon()));
		expect(lt(std::abs(test({ -1., 100., 100. })), std::numeric_limits<double>::epsilon()));
		expect(lt(std::abs(test({ 3., 3., 2. }) - 256), std::numeric_limits<double>::epsilon()));

		test = MathParser<double>("x y t : x * cos(y) / exp(t) + 10");
		expect(test.get_polish_notation() == "xycos*texp/10+");
		expect(test.get_n_vars() == 3);
		expect(std::abs(test({ 2., pi, 10. }) - 9.99991) < 1e-6);
	};

};

}