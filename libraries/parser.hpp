#pragma once

#include <unordered_map>
#include <stack>

#include "utils.hpp"
#include "expression.hpp"

// pre_infix_notation -> |variables : expression|. Example: |x y z t : x * y * z - t / x + sin(x * y * z)|.
// infix notation (standart) -> x + 5 * (y - z / t), polish notation (prefix) -> x 5 y z t / - * +
namespace parser {

namespace {
    using namespace std::string_literals;
    std::unordered_map<std::string, std::size_t> get_arithmetic_operators() {
		return std::move(std::unordered_map<std::string, std::size_t>{  {"+"s, 0}, {"-"s, 1}, {"*"s, 2},
																		{"/"s, 3}, {"^"s, 4}, {"~"s, 5}, {"sin"s, 6},
																		{"cos"s, 7}, {"tan"s, 8}, {"atan"s, 9}, {"exp"s, 10},
																		{"abs"s, 11}, {"sign"s, 12}, {"sqr"s, 13}, {"sqrt"s, 14},
																		{"log"s, 15} });
	}

}

class MathParser {
public:
    explicit MathParser(std::string pre_infix_notation);

    std::string get_polish_notation() const;
    std::size_t get_n_vars() const;
    std::unordered_map<std::string, std::size_t> get_vars() const;

    template <typename T>
    T operator()(const std::vector<T>& input_vars) const {
        return T(0);
    };

    template <utils::arithmetic T>
    T operator()(const std::vector<T>& input_vars) const {
        if (input_vars.size() != _vars.size())
            utils::error("Wrong number of variables");
	    return calc_polish_notation(input_vars);
    }

    template <utils::arithmetic T>
    T operator()(const std::initializer_list<T>& input_vars) const {
        if (input_vars.size() != _vars.size())
            utils::error("Wrong number of variables");
        std::vector<T> input(input_vars);
	    return calc_polish_notation(input);
    }

private:
    template<utils::arithmetic T>
	T execute(std::stack<T>& num_and_var, std::string& op) const {
		const auto ops = get_arithmetic_operators();
		const T right = num_and_var.empty() ? 0 : num_and_var.top();
		num_and_var.pop();
		T left = 0;
		switch(ops.at(op))
		{
		case 0:
			left = num_and_var.empty() ? 0 : num_and_var.top();
			num_and_var.pop();
			return left + right;
			break;
		case 1:
			left = num_and_var.empty() ? 0 : num_and_var.top();
			num_and_var.pop();
			return left - right;
			break;
		case 2:
			left = num_and_var.empty() ? 0 : num_and_var.top();
			num_and_var.pop();
			return left * right;
			break;
		case 3:
			left = num_and_var.empty() ? 0 : num_and_var.top();
			num_and_var.pop();
			return left / right;
			break;
		case 4:
			left = num_and_var.empty() ? 0 : num_and_var.top();
			num_and_var.pop();
			return std::pow(left, right);
			break;
		case 5:
			return -right;
			break;
		case 6:
			return std::sin(right);
			break;
		case 7:
			return std::cos(right);
			break;
		case 8:
			return std::tan(right);
			break;
		case 9:
			return std::atan(right);
			break;
		case 10:
			return std::exp(right);
			break;
		case 11:
			return std::abs(right);
			break;
		case 12:
			return (right > 0) ? 1 : ((right < 0) ? -1 : 0);
			break;
		case 13:
			return right * right;
			break;
		case 14:
			return std::sqrt(right);
			break;
		case 15:
			return std::log(right);
			break;
		default:
			utils::error("Error. Undefined operator <" + op + ">/.");
			return 0;
			break;
		}
		return 0;
	}

    template <utils::arithmetic T>
    T calc_polish_notation(const std::vector<T>& input_vars) const {
        const auto arithmetic_operators = get_arithmetic_operators();
        if (input_vars.size() != _vars.size())
            utils::error("Wrong number of input variables");
        std::stack<T> num_and_var;
        for (std::size_t i = 0; i < _polish_notation.size(); i++) {
            std::string smth = _polish_notation[i];
            if (utils::is_number(smth)) {
                num_and_var.push(utils::get_number<T>(smth));
            }
            else if (_vars.find(smth) != _vars.end()) {
                num_and_var.push(input_vars[_vars.at(smth)]);
            }
            else if (arithmetic_operators.count(smth) > 0) {
                num_and_var.push(execute(num_and_var, smth));
            }
        }
        return num_and_var.top();
    }

    std::unordered_map<std::string, std::size_t> get_variables(std::string& pre_variables) const;
    std::vector<std::pair<std::size_t, std::string>> find_vars_and_ops(const std::string& infix_notation) const;
    void assemble_polish_notation(const std::string& infix_notation, const std::vector<std::pair<std::size_t, std::string>>& var_op_indices);

    std::vector<std::string> _polish_notation{};
    std::unordered_map<std::string, std::size_t> _vars;
};

};

