#include "parser.h"

int main(int argc, char** argv) {

	std::array in{ 1 };
	variable<0> x;
	const double sigma = 1, M = 1;
	const auto normal_dist = _(1) / (_(2 * M_PI * sigma)) * exp(_(-0.5) * sqr((x - _(M)) / _(sigma)));
	std::cout << normal_dist(in) << std::endl;

}