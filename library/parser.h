#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <set>
#include <stack>

#include "utils.h"
#include "expression.h"

// pre_infix_notation -> |variables : expression|. Example: |x y z t : x * y * z - t / x + sin(x * y * z)|.
// infix notation (standart) -> x + 5 * (y - z / t), polish notation (prefix) -> x 5 y z t / - * +

template <arithmetic T = double>
class MathParser {
public:
    explicit MathParser(const str& pre_infix_notation);

    str get_polish_notation() const;
    size_type get_n_vars() const;
    std::map<str, std::pair<size_type, T>> get_vars() const;

    T calc_polish_notation(const std::vector<T>& input_vars);

    auto assemble_expression();

    T operator()(const std::vector<T>& input_vars);
    // template<arithmetic... Vars>
    // T operator()(Vars... vars);

    MathParser& operator =(const MathParser<T> from) {
        _pre_infix_notation = from._pre_infix_notation;
        _n_vars = from._n_vars;
        _infix_notation = from._infix_notation;
        _polish_notation = from._polish_notation;
        _vars = from._vars;
        _var_op_indices = from._var_op_indices;
        return *this;
    }

private:
    void set_variables(const std::vector<T>& input_vars);
    std::map<str, std::pair<size_type, T>> get_variables(str& pre_variables) const;
    void parentheses_check() const;
    void dots_check() const;

    void find_vars_and_ops(const str& notation);
    void assemble_polish_notation();

    str _pre_infix_notation;
    size_type _n_vars = 0;
    str _infix_notation;
    std::vector<str> _polish_notation{};
    std::map<str, std::pair<size_type, T>> _vars;
    std::vector<std::pair<size_type, str>> _var_op_indices;
    const std::map<str, size_type> _operator_priority{ {str{'('}, 0}, {str{'+'}, 1}, {str{'-'}, 1}, {str{'*'}, 2},
                                                       {str{'/'}, 2}, {str{'^'}, 3}, {str{'~'}, 4}, {str{"sin"}, 4}, 
                                                       {str{"cos"}, 4}, {str{"tan"}, 4}, {str{"atan"}, 4}, {str{"exp"}, 4},
                                                       {str{"abs"}, 4}, {str{"sign"}, 4}, {str{"sqr"}, 4}, {str{"sqrt"}, 4},
                                                       {str{"log"}, 4} };
    const std::set<symbol> _one_sym_operator = {'(', ')', '+', '-', '*', '/', '~', '^'};
};

template class MathParser<double>;

#endif
