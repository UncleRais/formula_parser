#include "parser.h"

namespace {
using namespace std::string_literals;
const std::unordered_map<std::string, std::size_t> _operator_priority{ {"("s, 0}, {"+"s, 1}, {"-"s, 1}, {"*"s, 2},
																	   {"/"s, 2}, {"^"s, 3}, {"~"s, 4}, {"sin"s, 4}, 
				                                                       {"cos"s, 4}, {"tan"s, 4}, {"atan"s, 4}, {"exp"s, 4},
																	   {"abs"s, 4}, {"sign"s, 4}, {"sqr"s, 4}, {"sqrt"s, 4},
																	   {"log"s, 4} };
}; 

template <arithmetic T>
MathParser<T>::MathParser(const std::string& pre_infix_notation) : _pre_infix_notation(pre_infix_notation) {
	std::size_t delimiter = _pre_infix_notation.find(':');
	if (delimiter == -1)
		error("Wrong variables format. Symbol ':' is required after variables initialization.");
	std::string pre_vars = _pre_infix_notation.substr(0, delimiter);
	_vars = get_variables(pre_vars);
	_n_vars = _vars.size();
	_infix_notation = _pre_infix_notation.substr(delimiter, -1);
	_infix_notation = delete_all(delete_all(_infix_notation, ' '), ':');
	if (_infix_notation.size() == 0)
		error("Wrong expression format. Formula after ':' is required.");
	parentheses_check();
	dots_check();

	find_vars_and_ops(_infix_notation);
	assemble_polish_notation();
}

template <arithmetic T>
std::string MathParser<T>::get_polish_notation() const {
	std::string res{};
	for (const auto& s : _polish_notation)
		res += s;
	return res;
}

template <arithmetic T>
std::size_t MathParser<T>::get_n_vars() const {
	return _n_vars;
}

template <arithmetic T>
std::map<std::string, std::pair<std::size_t, T>> MathParser<T>::get_vars() const {
	return _vars;
}

template<arithmetic T>
T execute(std::stack<T>& num_and_var, std::string& op) {
	const std::map<std::string, std::size_t> ops = { {"+"s, 0}, {"-"s, 1}, {"*"s, 2},
													 {"/"s, 3}, {"^"s, 4}, {"~"s, 5}, {"sin"s, 6},
													 {"cos"s, 7}, {"tan"s, 8}, {"atan"s, 9}, {"exp"s, 10},
													 {"abs"s, 11}, {"sign"s, 12}, {"sqr"s, 13}, {"sqrt"s, 14},
													 {"log"s, 15} };

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
		error("Error. Undefined operator <" + op + ">/.");
		break;
	}
}

template <arithmetic T>
T MathParser<T>::calc_polish_notation(const std::vector<T>& input_vars) {
	set_variables(input_vars);
	std::stack<T> num_and_var;
	for (std::size_t i = 0; i < _polish_notation.size(); i++) {
		std::string smth = _polish_notation[i];
		if (is_number(smth)) {
			num_and_var.push(get_number<T>(smth));
		}
		else if (_vars.find(smth) != _vars.end()) {
			num_and_var.push(_vars.at(smth).second);
		}
		else if (_operator_priority.count(smth) > 0) {
			num_and_var.push(execute(num_and_var, smth));
		}
	}
	return num_and_var.top();
}

//template <typename T>
//auto execute(std::stack<std::string>& num_and_var, std::map<std::string, std::size_t>& vars, std::string& op) {
//	const std::map<std::string, std::size_t> ops = { {'+'}, 0}, {'-'}, 1}, {'*'}, 2},
//													 {'/'}, 3}, {'^'}, 4}, {'~'}, 5}, {"sin"}, 6},
// 													 {"cos"}, 7}, {"tan"}, 8}, {"atan"}, 9}, {"exp"}, 10},
// 													 {"abs"}, 11}, {"sign"}, 12}, {"sqr"}, 13}, {"sqrt"}, 14},
//												     {"log"}, 15} };
//
//	auto get_expr = [&](const std::string& s){
//		if (vars.find(s) != vars.end()) {
//			return variable<vars[s]>();
//		}
//		else {
//			is_scalar<T>(get_number<T>(s));
//		}
//	};
//
//	const std::string right = num_and_var.empty() ? 0 : num_and_var.top();
//	num_and_var.pop();
//	std::string left = 0;
//	switch (ops.at(op))
//	{
//	case 0:
//		left = num_and_var.empty() ? 0 : num_and_var.top();
//		num_and_var.pop();
//		return (get_expr(left) + get_expr(right)).self();
//		break;
//	case 1:
//		left = num_and_var.empty() ? 0 : num_and_var.top();
//		num_and_var.pop();
//		return (get_expr(left) - get_expr(right)).self();
//		break;
//	case 2:
//		left = num_and_var.empty() ? 0 : num_and_var.top();
//		num_and_var.pop();
//		return (get_expr(left) * get_expr(right)).self();
//		break;
//	case 3:
//		left = num_and_var.empty() ? 0 : num_and_var.top();
//		num_and_var.pop();
//		return (get_expr(left) / get_expr(right)).self();
//		break;
//	case 4:
//		left = num_and_var.empty() ? 0 : num_and_var.top();
//		num_and_var.pop();
//		return (pow(get_expr(left), get_expr(right))).self();
//		break;
//	case 5:
//		return -get_expr(left).self();
//		break;
//	case 6:
//		return sin(get_expr(right)).self();
//		break;
//	case 7:
//		return cos(get_expr(right)).self();
//		break;
//	case 8:
//		return tan(get_expr(right)).self();
//		break;
//	case 9:
//		return ctan(get_expr(right)).self();
//		break;
//	case 10:
//		return exp(get_expr(right)).self();
//		break;
//	case 11:
//		return abs(get_expr(right)).self();
//		break;
//	case 12:
//		return sign(get_expr(right)).self();
//		break;
//	case 13:
//		return sqr(get_expr(right)).self();
//		break;
//	case 14:
//		return sqrt(get_expr(right)).self();
//		break;
//	case 15:
//		return log(get_expr(right)).self();
//		break;
//	default:
//		error("Error. Undefined operator <" + op + ">/.");
//		break;
//	}
//}

//template <typename T>
//auto MathParser<T>::assemble_expression() {
//	std::map<std::string, std::size_t> vars;
//	for (auto& [var, p] : _vars) 
//		vars[var] = p.first;
//
//	auto res = int_constant<0>();
//	std::stack<std::string> num_and_var;
//	for (std::size_t i = 0; i < _polish_notation.size(); i++) {
//		std::string smth = _polish_notation[i];
//		if (is_number(smth) || _vars.find(smth) != _vars.end()) {
//			num_and_var.push(smth);
//		}
//		else if (_operator_priority.count(smth) > 0) {
//			res += execute<T>(num_and_var, vars, smth);
//		}
//	}
//	return res;
//};

template <arithmetic T>
T MathParser<T>::operator()(const std::vector<T>& input_vars) {
	return calc_polish_notation(input_vars);
}

// template<arithmetic T>
// template<arithmetic... Vars>
// T MathParser<T>::operator()(Vars... vars) {
// 	std::vector<T> input{ vars... };
// 	if (input.size() != _n_vars)
// 		error("Wrong number of input variables.");
// 	return calc_polish_notation(input);
// }

template <arithmetic T>
void MathParser<T>::set_variables(const std::vector<T>& input_vars) {
	if (input_vars.size() != _vars.size())
		error("Wrong number of input variables");
	for (auto& [var, p] : _vars)
		p.second = input_vars[p.first];
}

void variables_format_check(const std::vector<std::string> variables) {
	for (const std::string& var : variables) {
		const char& first_variable_symbol = var[0];
		if (!is_latin_symbol(first_variable_symbol))
			error("Wrong variables format <" + var + ">. Variables must start with latin letter, variable cannot start with a number.");
	}
}

template <arithmetic T>
std::map<std::string, std::pair<std::size_t, T>> MathParser<T>::get_variables(std::string& pre_variables) const {
	const char delimiter_symbol = ' ';
	std::vector<std::string> variables = {};
	std::size_t delimiter = pre_variables.find(delimiter_symbol);
	if (delimiter == -1)
		error("Wrong variables format. Variables must be separated by spaces.");
	while (pre_variables.size() > 0) {
		variables.push_back(pre_variables.substr(0, delimiter));
		pre_variables = delimiter < pre_variables.size() ? pre_variables.substr(delimiter + 1, -1) : "";
		delimiter = pre_variables.find(delimiter_symbol);
	}
	variables_format_check(variables);
	std::map<std::string, std::pair<std::size_t, T>> res;
	for (std::size_t i = 0; i < variables.size(); i++)
		res[variables[i]] = std::make_pair(i, T{0});
	return res;
}

template <arithmetic T>
void MathParser<T>::parentheses_check() const {
	std::size_t left_p = count_all(_infix_notation, '(');
	std::size_t right_p = count_all(_infix_notation, ')');
	if (!(left_p == right_p))
		error("Wrong expression format. The expression contains open parentheses.");
}

template <arithmetic T>
void MathParser<T>::dots_check() const {
	char s = '.';
	if (_infix_notation[0] == s || _infix_notation[_infix_notation.size() - 1] == s)
		error("Wrong expression format. The expression can not be started or finished with dot. Dots are only allowed in number representation.");
	for (std::size_t position = _infix_notation.find(s); position != -1; position = _infix_notation.find(s, position + 1)) {
		if (!is_digit(_infix_notation[position + 1]))
			error("Wrong expression format. The expression contains invalid dots. Dots are only allowed in number representation");
	}
}

template <arithmetic T>
void MathParser<T>::find_vars_and_ops(const std::string& notation) {
	auto actually_var = [&](std::size_t pos, const std::string& s) {
		std::size_t shift = s.size() - 1;
		bool is_not_boundary_pos = ((0 < pos && pos < notation.size() - 1 - shift)
			&& (_one_sym_operator.count(notation[pos - 1]) > 0 && _one_sym_operator.count(notation[pos + shift + 1]) > 0));
		bool is_boundary_pos = (pos == 0 && _one_sym_operator.count(notation[pos + shift + 1]) > 0) ||
			(pos == notation.size() - 1 - shift && _one_sym_operator.count(notation[pos - 1]) > 0);
		return is_boundary_pos || is_not_boundary_pos;
	};

	auto push_it = [&](const std::string& s) {
		for (std::size_t position = notation.find(s); position != -1; position = notation.find(s, position + s.size()))
			if ( actually_var(position, s) )
				_var_op_indices.emplace_back(std::make_pair(position, s));
	};

	for (const auto& [var, val] : _vars)
		push_it(var);

	for (const auto& [op, prior] : _operator_priority)
		if (_one_sym_operator.find(op[0]) == _one_sym_operator.end())
			push_it(op);

	std::sort(_var_op_indices.begin(), _var_op_indices.end(), [](std::pair<std::size_t, std::string>& left, std::pair<std::size_t, std::string>& right) {
		return left.first < right.first;
		});
	if (_var_op_indices.size() == 0) {
		_var_op_indices.emplace_back(std::make_pair(-1, std::string{"not_a_variable"}));
		std::cout << "Warning: expression does not depend on the variables." << std::endl;
	}
}

template <arithmetic T>
void MathParser<T>::assemble_polish_notation() {
	std::stack<std::string> operators;
	std::size_t count = 0;
	for (std::size_t i = 0; i < _infix_notation.size(); i++) {
		const char& s = _infix_notation[i];
		if (_var_op_indices[count].first == i) {
			const auto& [ind, smth] = _var_op_indices[count];
			if (_operator_priority.find(smth) != _operator_priority.end()) {
				while (!operators.empty() && (_operator_priority.at(operators.top()) >= _operator_priority.at(smth))) {
					_polish_notation.push_back(operators.top());
					operators.pop();
				}
				operators.push(smth);
			}
			else if (_vars.find(smth) != _vars.end()) {
				_polish_notation.push_back(smth);
			}
			else {
				error("Wrong variables and operators searching. Can not find contained string.");
			}
			i += smth.size() - 1;
			count++;
			if (count >= _var_op_indices.size()) count = 0;
		} else if (is_digit(s) || s == '.') {
			if (i == 0 || (!is_digit(_infix_notation[i - 1]) && _infix_notation[i - 1] != '.' && i > 0)) {
				_polish_notation.push_back(std::string{ s });
			}
			else {
				_polish_notation.back().push_back(s);
			}
		}
		else if (s == '(') {
			operators.push(std::string{ s });
		}
		else if (s == ')') {
			while (!operators.empty() && operators.top() != std::string{ '(' }) {
				_polish_notation.push_back(operators.top());
				operators.pop();
			}
			operators.pop();
		}
		else if (_one_sym_operator.find(s) != _one_sym_operator.end() && _operator_priority.count(std::string{s}) > 0) {
			std::string op = std::string{ s };
			// for unary minus
			if (op == std::string{ '-' } && (i == 0 || (i > 1 && _operator_priority.count(std::string{ _infix_notation[i - 1] }) > 0)))
				op = std::string{ '~' };

			while (!operators.empty() && (_operator_priority.at(operators.top()) >= _operator_priority.at(op))) {
				_polish_notation.push_back(operators.top());
				operators.pop();
			}
			operators.push(op);
		}
	}
	while (!operators.empty()) {
		_polish_notation.push_back(operators.top());
		operators.pop();
	}
}