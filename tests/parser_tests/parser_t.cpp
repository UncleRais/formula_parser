#include "parser.hpp"

#include <numbers>
#include <limits>

#include <boost/ut.hpp>

namespace {
using namespace boost::ut;
using namespace parser;
using namespace parser::ex;
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

    "polish_notation_general"_test = [] {
        auto test = MathParser("x : -x");
        expect(test.to_polish() == "x~" and test.variables_count() == 1);

        test = MathParser("x:-x");
        expect(test.to_polish() == "x~" and test.variables_count() == 1);

        test = MathParser("x : x + x - x / x * x");
        expect(test.to_polish() == "xx+xx/x*-" and test.variables_count() == 1);

        test = MathParser("x : sin(x)");
        expect(test.to_polish() == "xsin" and test.variables_count() == 1);
        expect(lt(std::abs(test({ pi / 4 }) - std::sin(pi / 4)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : cos(x)");
        expect(test.to_polish() == "xcos" and test.variables_count() == 1);
        expect(lt(std::abs(test({ pi / 4 }) - std::cos(pi / 4)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : tan(x)");
        expect(test.to_polish() == "xtan" and test.variables_count() == 1);
        expect(lt(std::abs(test({ pi / 4 }) - std::tan(pi / 4)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : atan(x)");
        expect(test.to_polish() == "xatan" and test.variables_count() == 1);
        expect(lt(std::abs(test({ pi / 4 }) - std::atan(pi / 4)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : abs(x)");
        expect(test.to_polish() == "xabs" and test.variables_count() == 1);
        expect(lt(std::abs(test({ -pi / 4 }) - std::abs(-pi / 4)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : sign(x)");
        expect(test.to_polish() == "xsign" and test.variables_count() == 1);
        expect(lt(std::abs(test({ -pi / 4 }) + 1), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : sqr(x)");
        expect(test.to_polish() == "xsqr" and test.variables_count() == 1);
        expect(lt(std::abs(test({ std::sqrt(2) }) - 2), std::numeric_limits<double>::epsilon() * 3));

        test = MathParser("x : sqrt(x)");
        expect(test.to_polish() == "xsqrt" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 5. }) - std::sqrt(5)), std::numeric_limits<double>::epsilon() * 3));

        test = MathParser("x : log(x)");
        expect(test.to_polish() == "xlog" and test.variables_count() == 1);
        expect(lt(std::abs(test({ e }) - 1), std::numeric_limits<double>::epsilon()));

        // remark: Euler Gamma function
        // for each natural number i = 1, 2, 3, ... tgamma(i) = (i - 1)! 
        test = MathParser("x : tgamma(x)");
        expect(test.to_polish() == "xtgamma" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 6. }) - 120.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : lgamma(x)");
        expect(test.to_polish() == "xlgamma" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 124.23 }) - std::lgamma(124.23)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : exp2(x)");
        expect(test.to_polish() == "xexp2" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 4. }) - 16.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : expm1(x)");
        expect(test.to_polish() == "xexpm1" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 0. }) - 0.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : log10(x)");
        expect(test.to_polish() == "xlog10" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 100. }) - 2.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : log2(x)");
        expect(test.to_polish() == "xlog2" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 4. }) - 2.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : log1p(x)");
        expect(test.to_polish() == "xlog1p" and test.variables_count() == 1);
        expect(lt(std::abs(test({ e - 1. }) - 1.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : cbrt(x)");
        expect(test.to_polish() == "xcbrt" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 27 }) - 3), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : asin(x)");
        expect(test.to_polish() == "xasin" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 1. }) - pi / 2.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : acos(x)");
        expect(test.to_polish() == "xacos" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 0. }) - pi / 2.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : sinh(x)");
        expect(test.to_polish() == "xsinh" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 0. }) - 0.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : cosh(x)");
        expect(test.to_polish() == "xcosh" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 0. }) - 1.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : tanh(x)");
        expect(test.to_polish() == "xtanh" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 0. }) - 0.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : asinh(x)");
        expect(test.to_polish() == "xasinh" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 0. }) - 0.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : acosh(x)");
        expect(test.to_polish() == "xacosh" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 1. }) - 0.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : atanh(x)");
        expect(test.to_polish() == "xatanh" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 0. }) - 0.), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : erf(x)");
        expect(test.to_polish() == "xerf" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 124.23 }) - std::erf(124.23)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : erfc(x)");
        expect(test.to_polish() == "xerfc" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 124.23 }) - std::erfc(124.23)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : trunc(x)");
        expect(test.to_polish() == "xtrunc" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 124.23 }) - std::trunc(124.23)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : floor(x)");
        expect(test.to_polish() == "xfloor" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 124.23 }) - std::floor(124.23)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : ceil(x)");
        expect(test.to_polish() == "xceil" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 124.23 }) - std::ceil(124.23)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : round(x)");
        expect(test.to_polish() == "xround" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 124.23 }) - std::round(124.23)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : x^(0.3415234)");
        expect(test.to_polish() == "x0.3415234^" and test.variables_count() == 1);
        expect(lt(std::abs(test({ e }) - std::pow(e, 0.3415234)), std::numeric_limits<double>::epsilon()));

        test = MathParser("x : 0.411313 + .5 - x");
        expect(test.to_polish() == "0.411313.5+x-" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 0.411313 }) - .5), std::numeric_limits<double>::epsilon()));

        // trivial literal arithmetics
        test = MathParser("x : -5 + 56.23424 - .51241 / 0.4321 * 4 / 10");
        expect(test.to_polish() == "5~56.23424+.512410.4321/4*10/-" and test.variables_count() == 1);
        expect(lt(std::abs(test({ 100. }) - 50.7599), 1e-5));

        // general routine, examples
        test = MathParser("v1 v2 v3 p1 p2 p3 m : (p1^2 + p2^2 + p3^2) / (2 * m) + .5 * (v1^2 + v2^2 + v3^2) ");
        expect(test.to_polish() == "p12^p22^+p32^+2m*/.5v12^v22^+v32^+*+" and test.variables_count() == 7);
        expect(lt(std::abs(test({ 1., 1., 1., 2., 2., 2., 2. }) - 4.5), 1e-8));

        test = MathParser("x a b : (x - 1)^(a - 1) * (x + 1)^(b + 1)");
        expect(test.to_polish() == "x1-a1-^x1+b1+^*");
        expect(test.variables_count() == 3);
        expect(lt(std::abs(test({ 1., 1., 1. }) - 4), std::numeric_limits<double>::epsilon()));
        expect(lt(std::abs(test({ 1., 100., 100. })), std::numeric_limits<double>::epsilon()));
        expect(lt(std::abs(test({ -1., 100., 100. })), std::numeric_limits<double>::epsilon()));
        expect(lt(std::abs(test({ 3., 3., 2. }) - 256), std::numeric_limits<double>::epsilon()));

        test = MathParser("x y t : x * cos(y) / exp(t) + 10");
        expect(test.to_polish() == "xycos*texp/10+");
        expect(test.variables_count() == 3);
        expect(std::abs(test({ 2., pi, 10. }) - 9.99991) < 1e-6);
    };

    "polish_notation_throws"_test = [] {
        using namespace std::string_literals;
        static const std::unordered_map<std::string, std::size_t> operator_priority{{"("s, 0}, {"+"s, 1}, {"-"s, 1}, {"*"s, 2},
                                                                                    {"/"s, 2}, {"^"s, 3}, {"~"s, 4}, {"sin"s, 4}, 
                                                                                    {"cos"s, 4}, {"tan"s, 4}, {"atan"s, 4}, {"exp"s, 4},
                                                                                    {"abs"s, 4}, {"sign"s, 4}, {"sqr"s, 4}, {"sqrt"s, 4},
                                                                                    {"log"s, 4} };
        // Wrong variables format. Symbol ':' is required after variables initialization.
        expect(throws([]() { auto test = MathParser("x y z x * y * z"s); }));

        // Wrong variables format. Variables must start with latin letter, variable cannot start with a number
        expect(throws([]() { auto test = MathParser("1x 2x 3x: 1x + 2x + 3x)"s); }));
        expect(nothrow([]() { auto test = MathParser("x1 x2 x3: x1 + x2 + x3"s); }));

        // Wrong expression format. Formula after ':' is required.
        expect(throws([]() { auto test = MathParser("x y z: "s); }));

        // Everything alright. Variables are not required if not used.
        expect(nothrow([]() { auto test = MathParser(": 2 * 2"s); }));
        expect(nothrow([]() { auto test = MathParser("x : 2 * 2"s); }));

        // TODO : implement verification for this case, when variable in infix notation is not used 
        auto test = MathParser("x : 2 * 2 * y"s);
        expect(test.to_polish() == "22**");
        // Must be throws
        expect(nothrow([]() { auto test = MathParser("x : 2 * 2 * y"s); }));
        // Euler gamma function is not already implemented. Must be throws
        expect(nothrow([]() { auto test = MathParser("x y z: Г(x) * Г(y) * Г(z)"s); }));

        // Invalid variable designation. Variable name <" + variable + "> is unavailable.
        for (const auto& [op, _] : operator_priority) 
            expect(throws([&op]() { auto test = MathParser(op + " : 2 * 2"s); }));
        
        // Wrong expression format. The expression contains open parentheses
        expect(throws([]() { auto test = MathParser("x y z: exp(x*y*z"s); }));
        expect(throws([]() { auto test = MathParser("x y z: exp x*y*z)"s); }));

        // Wrong expression format. The expression can not be started or finished with dot. Dots are only allowed in number representation.
        // TODO : 0. format
        expect(throws([]() { auto test = MathParser("x y z: .5"s); }));
        expect(throws([]() { auto test = MathParser("x y z: 0."s); }));
        // must be nothrow
        expect(throws([]() { auto test = MathParser("x y z: 0. * x / y"s); }));
        expect(nothrow([]() { auto test = MathParser("x y z: 0.5"s); }));
        expect(nothrow([]() { auto test = MathParser("x y z: 0.0"s); }));
        expect(nothrow([]() { auto test = MathParser("x y z: x* .5 / y"s); }));

    };

};

}