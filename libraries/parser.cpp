#include "parser.hpp"

#include <unordered_set>

namespace parser {

namespace {
    using namespace std::string_literals;
    std::unordered_map<std::string, std::size_t> operator_priority{};
    std::unordered_set<char> one_sym_operators{};

    const std::unordered_map<std::string, std::size_t>& get_operator_priority() {
        if (operator_priority.size() == 0) {
            operator_priority = std::unordered_map<std::string, std::size_t>{{"("s, 0}, {"+"s, 1}, {"-"s, 1}, {"*"s, 2},
                                                                             {"/"s, 2}, {"^"s, 3}, {"~"s, 4}, {"sin"s, 4}, 
                                                                             {"cos"s, 4}, {"tan"s, 4}, {"atan"s, 4}, {"exp"s, 4},
                                                                             {"abs"s, 4}, {"sign"s, 4}, {"sqr"s, 4}, {"sqrt"s, 4},
                                                                             {"log"s, 4} };
        }
        return operator_priority;
    }

    const std::unordered_set<char>& get_one_sym_operators() {
        if (one_sym_operators.size() == 0) {
            one_sym_operators = std::unordered_set<char>{'(', ')', '+', '-', '*', '/', '~', '^'};
        }
        return one_sym_operators;
    }
    
    void parentheses_check(const std::string& infix_notation) {
    std::size_t left_p = utils::count_all(infix_notation, '(');
    std::size_t right_p = utils::count_all(infix_notation, ')');
    if (!(left_p == right_p))
        utils::error("Wrong expression format. The expression contains open parentheses.");
    }

    void dots_check(const std::string& infix_notation) {
        char s = '.';
        if (infix_notation[0] == s || infix_notation[infix_notation.size() - 1] == s)
            utils::error("Wrong expression format. The expression can not be started or finished with dot. Dots are only allowed in number representation.");
        for (std::size_t position = infix_notation.find(s); position != -1; position = infix_notation.find(s, position + 1)) {
            if (!std::isdigit(infix_notation[position + 1]))
                utils::error("Wrong expression format. The expression contains invalid dots. Dots are only allowed in number representation");
        }
    }

    void variables_format_check(const std::vector<std::string> variables) {
        for (const std::string& var : variables) {
            const char& first_variable_symbol = var[0];
            if (!std::isalpha(first_variable_symbol))
                utils::error("Wrong variables format <" + var + ">. Variables must start with latin letter, variable cannot start with a number.");
        }
    }

}

MathParser::MathParser(std::string pre_infix_notation) {
    std::size_t delimiter = pre_infix_notation.find(':');
    if (delimiter == -1)
        utils::error("Wrong variables format. Symbol ':' is required after variables initialization.");
    std::string pre_vars = pre_infix_notation.substr(0, delimiter);
    _vars = get_variables(pre_vars);
    std::string infix_notation = pre_infix_notation.substr(delimiter, -1);
    infix_notation = utils::delete_all(utils::delete_all(infix_notation, ' '), ':');
    if (infix_notation.size() == 0)
        utils::error("Wrong expression format. Formula after ':' is required.");
    parentheses_check(infix_notation);
    dots_check(infix_notation);
    const std::vector<std::pair<std::size_t, std::string>> var_op_indices = find_vars_and_ops(infix_notation);
    assemble_polish_notation(infix_notation, var_op_indices);
}

std::string MathParser::get_polish_notation() const {
    auto concatenate = [](const std::string& a, const std::string& b){ return a + b; };
    return std::accumulate(_polish_notation.begin(), _polish_notation.end(), std::string{""}, concatenate);
}

std::size_t MathParser::get_n_vars() const {
    return _vars.size();
}

std::unordered_map<std::string,std::size_t> MathParser::get_vars() const {
    return _vars;
}

std::unordered_map<std::string, std::size_t> MathParser::get_variables(std::string& pre_variables) const {
    const char delimiter_symbol = ' ';
    std::vector<std::string> variables = {};
    std::size_t delimiter = pre_variables.find(delimiter_symbol);
    if (delimiter == -1)
        utils::error("Wrong variables format. Variables must be separated by spaces.");
    while (pre_variables.size() > 0) {
        variables.push_back(pre_variables.substr(0, delimiter));
        pre_variables = delimiter < pre_variables.size() ? pre_variables.substr(delimiter + 1, -1) : "";
        delimiter = pre_variables.find(delimiter_symbol);
    }
    variables_format_check(variables);
    std::unordered_map<std::string, std::size_t> res;
    for (std::size_t i = 0; i < variables.size(); i++)
        res[variables[i]] = i;
    return res;
}

std::vector<std::pair<std::size_t, std::string>> MathParser::find_vars_and_ops(const std::string& infix_notation) const {
    const auto& operator_priority = get_operator_priority();
    const auto& one_sym_operator = get_one_sym_operators();
    std::vector<std::pair<std::size_t, std::string>> var_op_indices;

    auto actually_var = [&](std::size_t pos, const std::string& s) {
        std::size_t shift = s.size() - 1;
        bool is_not_boundary_pos = ((0 < pos && pos < infix_notation.size() - 1 - shift)
            && (one_sym_operator.count(infix_notation[pos - 1]) > 0 && one_sym_operator.count(infix_notation[pos + shift + 1]) > 0));
        bool is_boundary_pos = (pos == 0 && one_sym_operator.count(infix_notation[pos + shift + 1]) > 0) ||
            (pos == infix_notation.size() - 1 - shift && one_sym_operator.count(infix_notation[pos - 1]) > 0);
        return is_boundary_pos || is_not_boundary_pos;
    };

    auto push_it = [&](const std::string& s) {
        for (std::size_t position = infix_notation.find(s); position != -1; position = infix_notation.find(s, position + s.size()))
            if ( actually_var(position, s) )
                var_op_indices.emplace_back(std::make_pair(position, s));
    };

    for (const auto& [var, val] : _vars)
        push_it(var);

    for (const auto& [op, prior] : operator_priority)
        if (one_sym_operator.find(op[0]) == one_sym_operator.end())
            push_it(op);

    std::sort(var_op_indices.begin(), var_op_indices.end(), [](std::pair<std::size_t, std::string>& left, std::pair<std::size_t, std::string>& right) {
        return left.first < right.first;
        });
    if (var_op_indices.size() == 0) {
        var_op_indices.emplace_back(std::make_pair(-1, std::string{"not_a_variable"}));
        std::cout << "Warning: expression does not depend on the variables." << std::endl;
    }
    return std::move(var_op_indices);
}

void MathParser::assemble_polish_notation(const std::string& infix_notation, const std::vector<std::pair<std::size_t, std::string>>& var_op_indices) {
    const auto& operator_priority = get_operator_priority();
    const auto& one_sym_operator = get_one_sym_operators();
    std::stack<std::string> operators;
    std::size_t count = 0;
    for (std::size_t i = 0; i < infix_notation.size(); i++) {
        const char& s = infix_notation[i];
        if (var_op_indices[count].first == i) {
            const auto& [ind, smth] = var_op_indices[count];
            if (operator_priority.find(smth) != operator_priority.end()) {
                while (!operators.empty() && (operator_priority.at(operators.top()) >= operator_priority.at(smth))) {
                    _polish_notation.push_back(operators.top());
                    operators.pop();
                }
                operators.push(smth);
            }
            else if (_vars.find(smth) != _vars.end()) {
                _polish_notation.push_back(smth);
            }
            else {
                utils::error("Wrong variables and operators searching. Can not find contained string.");
            }
            i += smth.size() - 1;
            count++;
            if (count >= var_op_indices.size()) count = 0;
        } else if (std::isdigit(s) || s == '.') {
            if (i == 0 || (!std::isdigit(infix_notation[i - 1]) && infix_notation[i - 1] != '.' && i > 0)) {
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
        else if (one_sym_operator.find(s) != one_sym_operator.end() && operator_priority.count(std::string{s}) > 0) {
            std::string op = std::string{ s };
            // for unary minus
            if (op == std::string{ '-' } && (i == 0 || (i > 1 && operator_priority.count(std::string{ infix_notation[i - 1] }) > 0)))
                op = std::string{ '~' };

            while (!operators.empty() && (operator_priority.at(operators.top()) >= operator_priority.at(op))) {
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

}; 