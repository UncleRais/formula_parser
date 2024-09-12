#include "tests.h"

int main(int argc, char** argv) {
    test_arithmetics();
    test_functions();
    test_polish_notation();
    std::cout << " Success." << std::endl;
    return 0;
}