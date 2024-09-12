#ifndef PARSER_CPP
#define PARSER_CPP

#include "parser.h"

template <arithmetic T>
MathParser<T>::MathParser(const str& pre_infix_notation) : _pre_infix_notation(pre_infix_notation) {
	size_type delimiter = _pre_infix_notation.find(':');
	if (delimiter == -1)
		error("Wrong variables format. Symbol ':' is required after variables initialization.");
	str pre_vars = _pre_infix_notation.substr(0, delimiter);
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
str MathParser<T>::get_polish_notation() const {
	str res{};
	for (const auto& s : _polish_notation)
		res += s;
	return res;
}

template <arithmetic T>
size_type MathParser<T>::get_n_vars() const {
	return _n_vars;
}

template <arithmetic T>
std::map<str, std::pair<size_type, T>> MathParser<T>::get_vars() const {
	return _vars;
}

template<arithmetic T>
T execute(std::stack<T>& num_and_var, str& op) {
	const std::map<str, size_type> ops = { {str{'+'}, 0}, {str{'-'}, 1}, {str{'*'}, 2},
											{str{'/'}, 3}, {str{'^'}, 4}, {str{'~'}, 5}, {str{"sin"}, 6},
											{str{"cos"}, 7}, {str{"tan"}, 8}, {str{"atan"}, 9}, {str{"exp"}, 10},
											{str{"abs"}, 11}, {str{"sign"}, 12}, {str{"sqr"}, 13}, {str{"sqrt"}, 14},
											{str{"log"}, 15} };

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
	for (size_type i = 0; i < _polish_notation.size(); i++) {
		str smth = _polish_notation[i];
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
//auto execute(std::stack<str>& num_and_var, std::map<str, size_type>& vars, str& op) {
//	const std::map<str, size_type> ops = { {str{'+'}, 0}, {str{'-'}, 1}, {str{'*'}, 2},
//										   {str{'/'}, 3}, {str{'^'}, 4}, {str{'~'}, 5}, {str{"sin"}, 6},
//										   {str{"cos"}, 7}, {str{"tan"}, 8}, {str{"atan"}, 9}, {str{"exp"}, 10},
//										   {str{"abs"}, 11}, {str{"sign"}, 12}, {str{"sqr"}, 13}, {str{"sqrt"}, 14},
//										   {str{"log"}, 15} };
//
//	auto get_expr = [&](const str& s){
//		if (vars.find(s) != vars.end()) {
//			return variable<vars[s]>();
//		}
//		else {
//			is_scalar<T>(get_number<T>(s));
//		}
//	};
//
//	const str right = num_and_var.empty() ? 0 : num_and_var.top();
//	num_and_var.pop();
//	str left = 0;
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
//	std::map<str, size_type> vars;
//	for (auto& [var, p] : _vars) 
//		vars[var] = p.first;
//
//	auto res = int_constant<0>();
//	std::stack<str> num_and_var;
//	for (size_type i = 0; i < _polish_notation.size(); i++) {
//		str smth = _polish_notation[i];
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

void variables_format_check(const std::vector<str> variables) {
	for (const str& var : variables) {
		const symbol& first_variable_symbol = var[0];
		if (!is_latin_symbol(first_variable_symbol))
			error("Wrong variables format <" + var + ">. Variables must start with latin letter, variable cannot start with a number.");
	}
}

template <arithmetic T>
std::map<str, std::pair<size_type, T>> MathParser<T>::get_variables(str& pre_variables) const {
	const symbol delimiter_symbol = ' ';
	std::vector<str> variables = {};
	size_type delimiter = pre_variables.find(delimiter_symbol);
	if (delimiter == -1)
		error("Wrong variables format. Variables must be separated by spaces.");
	while (pre_variables.size() > 0) {
		variables.push_back(pre_variables.substr(0, delimiter));
		pre_variables = delimiter < pre_variables.size() ? pre_variables.substr(delimiter + 1, -1) : "";
		delimiter = pre_variables.find(delimiter_symbol);
	}
	variables_format_check(variables);
	std::map<str, std::pair<size_type, T>> res;
	for (size_type i = 0; i < variables.size(); i++)
		res[variables[i]] = std::make_pair(i, T{0});
	return res;
}

template <arithmetic T>
void MathParser<T>::parentheses_check() const {
	size_type left_p = count_all(_infix_notation, '(');
	size_type right_p = count_all(_infix_notation, ')');
	if (!(left_p == right_p))
		error("Wrong expression format. The expression contains open parentheses.");
}

template <arithmetic T>
void MathParser<T>::dots_check() const {
	symbol s = '.';
	if (_infix_notation[0] == s || _infix_notation[_infix_notation.size() - 1] == s)
		error("Wrong expression format. The expression can not be started or finished with dot. Dots are only allowed in number representation.");
	for (size_type position = _infix_notation.find(s); position != -1; position = _infix_notation.find(s, position + 1)) {
		if (!is_digit(_infix_notation[position + 1]))
			error("Wrong expression format. The expression contains invalid dots. Dots are only allowed in number representation");
	}
}

template <arithmetic T>
void MathParser<T>::find_vars_and_ops(const str& notation) {
	auto actually_var = [&](size_type pos, const str& s) {
		size_type shift = s.size() - 1;
		bool is_not_boundary_pos = ((0 < pos && pos < notation.size() - 1 - shift)
			&& (_one_sym_operator.count(notation[pos - 1]) > 0 && _one_sym_operator.count(notation[pos + shift + 1]) > 0));
		bool is_boundary_pos = (pos == 0 && _one_sym_operator.count(notation[pos + shift + 1]) > 0) ||
			(pos == notation.size() - 1 - shift && _one_sym_operator.count(notation[pos - 1]) > 0);
		return is_boundary_pos || is_not_boundary_pos;
	};

	auto push_it = [&](const str& s) {
		for (size_type position = notation.find(s); position != -1; position = notation.find(s, position + s.size()))
			if ( actually_var(position, s) )
				_var_op_indices.emplace_back(std::make_pair(position, s));
	};

	for (const auto& [var, val] : _vars)
		push_it(var);

	for (const auto& [op, prior] : _operator_priority)
		if (_one_sym_operator.find(op[0]) == _one_sym_operator.end())
			push_it(op);

	std::sort(_var_op_indices.begin(), _var_op_indices.end(), [](std::pair<size_type, str>& left, std::pair<size_type, str>& right) {
		return left.first < right.first;
		});
	if (_var_op_indices.size() == 0) {
		_var_op_indices.emplace_back(std::make_pair(-1, str{"not_a_variable"}));
		std::cout << "Warning: expression does not depend on the variables." << std::endl;
	}
}

template <arithmetic T>
void MathParser<T>::assemble_polish_notation() {
	std::stack<str> operators;
	size_type count = 0;
	for (size_type i = 0; i < _infix_notation.size(); i++) {
		const symbol& s = _infix_notation[i];
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
				_polish_notation.push_back(str{ s });
			}
			else {
				_polish_notation.back().push_back(s);
			}
		}
		else if (s == '(') {
			operators.push(str{ s });
		}
		else if (s == ')') {
			while (!operators.empty() && operators.top() != str{ '(' }) {
				_polish_notation.push_back(operators.top());
				operators.pop();
			}
			operators.pop();
		}
		else if (_one_sym_operator.find(s) != _one_sym_operator.end() && _operator_priority.count(str{s}) > 0) {
			str op = str{ s };
			// for unary minus
			if (op == str{ '-' } && (i == 0 || (i > 1 && _operator_priority.count(str{ _infix_notation[i - 1] }) > 0)))
				op = str{ '~' };

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

#endif