#pragma once

#include "utils.hpp"
#include "expression.hpp"

#include <unordered_map>
#include <stack>
#include <span>
#include <stdexcept>

// pre_infix_notation -> |variables : expression|. Example: |x y z t : x * y * z - t / x + sin(x * y * z)|.
// infix notation (standart) -> x + 5 * (y - z / t), polish notation (prefix) -> x 5 y z t / - * +
namespace parser {

class MathParser {
public:
    explicit MathParser(std::string pre_infix_notation);

    std::string to_polish() const;
    std::size_t variables_count() const;
    std::unordered_map<std::string, std::size_t> get_variables() const;

    template <utils::arithmetic T>
    T operator()(const std::span<const T> input_vars) const {
        return calc_polish_notation(input_vars);
    }

    template <utils::arithmetic T>
    T operator()(const std::initializer_list<T>& input_vars) const {
        return this->operator()(std::span(input_vars));
    }

private:
    enum class operator_index : std::size_t {
        plus, minus, unary_minus,
        multiply, divide, power, sqr, sqrt,
        sin, cos, tan, atan,
        exp, log,
        abs, sign
    };

    template<utils::arithmetic T>
    T execute(const std::unordered_map<std::string, MathParser::operator_index>& ops, 
              std::stack<T>& calculation_values, const std::string& op) const {
        const auto pop_element = [&calculation_values]() {
            T elem = T(0);
            if (!calculation_values.empty()) {
                elem = calculation_values.top();
                calculation_values.pop();
            }
            return elem;
        };
        const T right = pop_element();
        switch(ops.at(op))
        {
        case operator_index::plus:
            return pop_element() + right;
        case operator_index::minus:
            return pop_element() - right;
        case operator_index::multiply:
            return pop_element() * right;
        case operator_index::divide:
            return pop_element() / right;
        case operator_index::power:
            return std::pow(pop_element(), right);
        case operator_index::unary_minus:
            return -right;
        case operator_index::sin:
            return std::sin(right);
        case operator_index::cos:
            return std::cos(right);
        case operator_index::tan:
            return std::tan(right);
        case operator_index::atan:
            return std::atan(right);
        case operator_index::exp:
            return std::exp(right);
        case operator_index::abs:
            return std::abs(right);
        case operator_index::sign:
            return (right > 0) - (right < 0);
        case operator_index::sqr:
            return right * right;
        case operator_index::sqrt:
            return std::sqrt(right);
        case operator_index::log:
            return std::log(right);
        default:
            throw std::domain_error{"Error. Undefined operator <" + op + ">/."};
        }
    }

    template <utils::arithmetic T>
    T calc_polish_notation(const std::span<const T> input_variables) const {
        if (input_variables.size() != _variables.size()) [[unlikely]]
            throw std::domain_error{"Wrong number of variables."};
        const auto& arithmetic_operators = get_arithmetic_operators();
        std::stack<T> calculation_values;
        for (const std::string& smth : _polish_notation) {
            if (utils::is_number(smth)) {
                calculation_values.push(utils::get_number<T>(smth));
            }
            else if (_variables.count(smth) > 0) {
                calculation_values.push(input_variables[_variables.at(smth)]);
            }
            else if (arithmetic_operators.count(smth) > 0) {
                calculation_values.push(execute<T>(arithmetic_operators, calculation_values, smth));
            }
        }
        return calculation_values.top();
    }

    static const std::unordered_map<std::string, operator_index>& get_arithmetic_operators();
    std::unordered_map<std::size_t, std::string> find_variables_and_operators(const std::string& infix_notation) const;
    void assemble_polish_notation(const std::string& infix_notation);

    std::vector<std::string> _polish_notation{};
    std::unordered_map<std::string, std::size_t> _variables;
};

};

