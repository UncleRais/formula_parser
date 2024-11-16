#pragma once

#include <unordered_map>

#include "utils.hpp"
#include "expression.hpp"

// pre_infix_notation -> |variables : expression|. Example: |x y z t : x * y * z - t / x + sin(x * y * z)|.
// infix notation (standart) -> x + 5 * (y - z / t), polish notation (prefix) -> x 5 y z t / - * +
namespace parser {

template <arithmetic T = double>
class MathParser {
public:
    explicit MathParser(const std::string& pre_infix_notation);

    std::string get_polish_notation() const;
    std::size_t get_n_vars() const;
    std::unordered_map<std::string, std::pair<std::size_t, T>> get_vars() const;

    T calc_polish_notation(const std::vector<T>& input_vars);

    auto assemble_expression();

    T operator()(const std::vector<T>& input_vars);

private:
    void set_variables(const std::vector<T>& input_vars);
    std::unordered_map<std::string, std::pair<std::size_t, T>> get_variables(std::string& pre_variables) const;
    void parentheses_check() const;
    void dots_check() const;

    void find_vars_and_ops(const std::string& notation);
    void assemble_polish_notation();

    std::string _pre_infix_notation;
    std::size_t _n_vars = 0;
    std::string _infix_notation;
    std::vector<std::string> _polish_notation{};
    std::unordered_map<std::string, std::pair<std::size_t, T>> _vars;
    std::vector<std::pair<std::size_t, std::string>> _var_op_indices;
};

template class MathParser<double>;

};

