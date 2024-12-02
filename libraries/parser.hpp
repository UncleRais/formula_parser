#pragma once

#include <unordered_map>
#include <stack>
#include <span>
#include <stdexcept>

#include "utils.hpp"
#include "expression.hpp"
#include <iostream>

// pre_infix_notation -> |variables : expression|. Example: |x y z t : x * y * z - t / x + sin(x * y * z)|.
// infix notation (standart) -> x + 5 * (y - z / t), polish notation (prefix) -> x 5 y z t / - * +
namespace parser {

class MathParser {
public:
    explicit MathParser(std::string pre_infix_notation);

    std::string get_polish_notation() const;
    std::size_t get_n_vars() const;
    std::unordered_map<std::string, std::size_t> get_vars() const;

    template <utils::arithmetic T>
    T operator()(const std::span<const T> input_vars) const {
        return calc_polish_notation(input_vars);
    }

    template <utils::arithmetic T>
    T operator()(const std::initializer_list<T>& input_vars) const {
        return this->operator()(std::span(input_vars));
    }

private:
    enum class op_idx : std::size_t {
        plus = 0, 
        minus = 1,
        multiply = 2,
        divide = 3,
        power = 4,
        unary_minus = 5,
        sin = 6,
        cos = 7, 
        tan = 8,
        atan = 9,
        exp = 10,
        abs = 11,
        sign = 12,
        sqr = 13,
        sqrt = 14,
        log = 15
    };

    template<utils::arithmetic T>
    T execute(const std::unordered_map<std::string, MathParser::op_idx>& ops, std::stack<T>& num_and_var, const std::string& op) const {
        auto pop_element = [&num_and_var]() {
            T elem = T(0);
            if (!num_and_var.empty()) {
                elem = num_and_var.top();
                num_and_var.pop();
            }
            return elem;
        };
        const T right = pop_element();
        switch(ops.at(op))
        {
        case op_idx::plus:
            return pop_element() + right;
        case op_idx::minus:
            return pop_element() - right;
        case op_idx::multiply:
            return pop_element() * right;
        case op_idx::divide:
            return pop_element() / right;
        case op_idx::power:
            return std::pow(pop_element(), right);
        case op_idx::unary_minus:
            return -right;
        case op_idx::sin:
            return std::sin(right);
        case op_idx::cos:
            return std::cos(right);
        case op_idx::tan:
            return std::tan(right);
        case op_idx::atan:
            return std::atan(right);
        case op_idx::exp:
            return std::exp(right);
        case op_idx::abs:
            return std::abs(right);
        case op_idx::sign:
            return (right > 0) - (right < 0);
        case op_idx::sqr:
            return right * right;
        case op_idx::sqrt:
            return std::sqrt(right);
        case op_idx::log:
            return std::log(right);
        default:
            throw std::domain_error{"Error. Undefined operator <" + op + ">/."};
        }
    }

    template <utils::arithmetic T>
    T calc_polish_notation(const std::span<const T> input_vars) const {
        if (input_vars.size() != _vars.size()) [[unlikely]]
            throw std::domain_error{"Wrong number of variables."};
        const auto& arithmetic_operators = get_arithmetic_operators();
        std::stack<T> num_and_var;
        for (const std::string& smth : _polish_notation) {
            if (utils::is_number(smth)) {
                num_and_var.push(utils::get_number<T>(smth));
            }
            else if (_vars.count(smth) > 0) {
                num_and_var.push(input_vars[_vars.at(smth)]);
            }
            else if (arithmetic_operators.count(smth) > 0) {
                num_and_var.push(execute<T>(arithmetic_operators, num_and_var, smth));
            }
        }
        return num_and_var.top();
    }

    static const std::unordered_map<std::string, op_idx>& get_arithmetic_operators();
    std::unordered_map<std::string, std::size_t> get_variables(std::string pre_variables) const;
    std::vector<std::pair<std::size_t, std::string>> find_vars_and_ops(const std::string& infix_notation) const;
    void assemble_polish_notation(const std::string& infix_notation, const std::vector<std::pair<std::size_t, std::string>>& var_op_indices);

    std::vector<std::string> _polish_notation{};
    std::unordered_map<std::string, std::size_t> _vars;
};

};

