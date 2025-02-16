# Formula parser
## Author :  [Safronov Yury](https://github.com/UncleRais)
## Education : [Applied Mathematics department of BMSTU](http://fn.bmstu.ru/tm-fs-2)

Contains implementations of:
* compile time formula arithmetics;
* run time formula parser.

# Compile time arithmetics

We formulate simple grammatics for domain specific language (DSL)
* expression := constant | variable | expression +*/- expression | FUNCTION(expression) | (expression) | -expression.
Grammatic is used to implement expression class.

# How to use
## Example №1
```c++
#include "expression.hpp"

int main(int argc, char** argv) {
	std::array in{ 1 };
	variable<0> x;
	const double sigma = 1, M = 1;
	const auto normal_dist = _(1) / (_(2 * M_PI * sigma)) * exp(_(-0.5) * sqr((x - _(M)) / _(sigma)));
	std::cout << normal_dist(in) << std::endl;
	return 0;
}
```

## Example №2
```c++
#include "expression.hpp"

int main(int argc, char** argv) {
	std::array in{ 1, 2, 3 };
	variable<0> x;
  	variable<1> y;
  	variable<2> z;
	const auto vector_norm = x * x + y * y + z * z;
	std::cout << vector_norm(in) << std::endl;
	return 0;
}
```

# Run time parser
It converts infix notation to polish notation in order to further substituting variables into corresponding formula and its evaluation.
Special input format is required: 
* Simple scheme : pre_infix_notation -> |variables : expression|. Example: |x y z t : x * y * z - t / x + sin(x * y * z)|.
* Infix notation (standart) -> x + 5 * (y - z / t), polish notation (prefix) -> x 5 y z t / - * +.

# How to use
## Example №1
```c++
#include "parser.hpp"

int main(int argc, char** argv) {
	auto test = MathParser("v1 v2 v3 p1 p2 p3 m : (p1^2 + p2^2 + p3^2) / (2 * m) + .5 * (v1^2 + v2^2 + v3^2) ");
	assert(test.get_polish_notation() == "p12^p22^+p32^+2m*/.5v12^v22^+v32^+*+" && test.get_n_vars() == 7);
	assert(std::abs(test({ 1, 1, 1, 2, 2, 2, 2 }) - 4.5) < 1e-8);
	
	test = MathParser("x a b : (x - 1)^(a - 1) * (x + 1)^(b + 1)");
	assert(test.get_polish_notation() == "x1-a1-^x1+b1+^*");
	assert(test.get_n_vars() == 3);
	assert(std::abs(test({ 1., 1., 1. }) - 4) < 1e-15);
	assert(std::abs(test({ 1., 100., 100. })) < 1e-15);
	assert(std::abs(test({ -1., 100., 100. })) < 1e-15);
	assert(std::abs(test({ 3., 3., 2. }) - 256) < 1e-15);
	
	test = MathParser("x y t : x * cos(y) / exp(t) + 10");
	assert(test.get_polish_notation() == "xycos*texp/10+");
	assert(test.get_n_vars() == 3);
	assert(std::abs(test({ 2., M_PI, 10. }) - 9.99991) < 1e-5);
	return 0;
}
```

You can use MathParser as ordinary scalar function of vector argument.
