#pragma once
#include "utils.hpp"

#include <math.h>
#include <vector>
#include <array>

namespace parser::ex {
// Grammatics for Domain specific language (DSL)
// expression := constant | variable | expression +*/- expression | FUNCTION(expression) | (expression) | -expression 
template<class E>
struct math_object_base {
    E& self() {
        return static_cast<E&> (*this);
    }
    const E& self() const {
        return static_cast<const E&> (*this);
    }
};

// This class does not contain functionality. It takes final class template and translates it to the base class.
template<class E>
struct expression : math_object_base<E> {};
// ----------------- Constants and variables ----------------- 
// Integral type constants
template<int N>
struct int_constant : expression<int_constant<N>> {
    static constexpr int value = N;

    template<typename T>
    int operator()(const T& x) const {
        return value;
    }
};
template<class E>
struct is_int_constant : std::false_type {};
template<int N>
struct is_int_constant<int_constant<N>> : std::true_type {};

template<class E>
struct int_constant_value : std::integral_constant<int, 0> {};
template<int N>
struct int_constant_value<int_constant<N>> : std::integral_constant<int, N> {};
// Other types
template<typename VT>
struct scalar : expression<scalar<VT>> {
    using value_type = VT;

    scalar(const value_type& value) : value(value) {}

    template <typename T>
    value_type operator()(const T& x) const {
        return value;
    }
    const value_type value;
};
template <class E>
struct is_scalar : std::false_type {};
template <typename VT>
struct is_scalar<scalar<VT>> : std::true_type {};

template<typename T>
scalar<T> _(const T& val) {
    return scalar<T>(val);
}
// Variable 
template<std::size_t N>
struct variable : expression<variable<N>> {
    template<typename T, std::size_t _Size>
    T operator()(const std::array<T, _Size>& vars) const {
        return vars[N];
    }
};
// ----------------------------------------------------------- 

// -------------------------Negate----------------------------
template<class E>
struct negate_expression;
// Compile time optimization for constants
template<class E>
struct negate_expression_type {
    using type = typename
        std::conditional_t<
            is_int_constant<E>::value,
            int_constant<-int_constant_value<E>::value>,
            typename std::conditional_t<
                is_scalar<E>::value, 
                E, 
                negate_expression<E>
            >
        >;
};
template<class E>
struct negate_expression : expression<negate_expression<E>> {

    negate_expression(const expression<E>& e) : e(e.self()) {}

    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return -e(x);
    }
    const E e;
};
template<class E>
negate_expression<E>
operator-(const expression<E>& e) {
    return negate_expression<E>(e);
}
template<int N>
int_constant<-N>
operator-(const int_constant<N>&) {
    return int_constant<-N>();
}
template<typename VT>
scalar<VT>
operator-(const scalar<VT>& e) {
    return scalar<VT>(-e.value);
}
// -----------------------------------------------------------
// --------------------------Operations-----------------------
template<class E1, char op, class E2>
struct binary_expression;

template<class E1, char op, class E2>
struct binary_expression : expression<binary_expression<E1, op, E2> >
{
    binary_expression(const expression<E1>& e1, const expression<E2>& e2) : e1(e1.self()), e2(e2.self()) {}

    template <typename T, std::size_t M, char _op = op, typename std::enable_if_t<_op == '+', bool> = false>
    T operator()(const std::array<T, M>& x) const {
        return e1(x) + e2(x);
    }
    template <typename T, std::size_t M, char _op = op, typename std::enable_if_t<_op == '-', bool> = false>
    T operator()(const std::array<T, M>& x) const {
        return e1(x) - e2(x);
    }
    template <typename T, std::size_t M, char _op = op, typename std::enable_if_t<_op == '*', bool> = false>
    T operator()(const std::array<T, M>& x) const {
        return e1(x) * e2(x);
    }
    template <typename T, std::size_t M, char _op = op, typename std::enable_if_t<_op == '/', bool> = false>
    T operator()(const std::array<T, M>& x) const {
        return e1(x) / e2(x);
    }
    const E1 e1;
    const E2 e2;
};
template<class E1, class E2>
binary_expression<E1, '+', E2>
operator +(const expression<E1>& e1, const expression<E2>& e2) {
    return binary_expression<E1, '+', E2>(e1, e2);
}
template<class E1, class E2>
binary_expression<E1, '-', E2>
operator -(const expression<E1>& e1, const expression<E2>& e2) {
    return binary_expression<E1, '-', E2>(e1, e2);
}
template<class E1, class E2>
binary_expression<E1, '*', E2>
operator *(const expression<E1>& e1, const expression<E2>& e2) {
    return binary_expression<E1, '*', E2>(e1, e2);
}
template<class E1, class E2>
binary_expression<E1, '/', E2>
operator /(const expression<E1>& e1, const expression<E2>& e2) {
    return binary_expression<E1, '/', E2>(e1, e2);
}

template<class E>
const E& operator+(const expression<E>& e, const int_constant<0>&) {
    return e.self();
}
template<class E>
int_constant<0> operator*(const expression<E>& e, const int_constant<0>&) {
    return int_constant<0>();
}
template<int N1, int N2>
int_constant<N1 + N2>
operator+(const int_constant<N1>&, const int_constant<N2>&) {
    return int_constant<N1 + N2>();
}
template<int N1, int N2>
int_constant<N1 - N2>
operator-(const int_constant<N1>&, const int_constant<N2>&) {
    return int_constant<N1 - N2>();
}
template<int N1, int N2>
int_constant<N1 * N2>
operator*(const int_constant<N1>&, const int_constant<N2>&) {
    return int_constant<N1 * N2>();
}
// -----------------------------------------------------------
// --------------------------Functions------------------------
// Trigonometry (sin, cos, tan, ctan)
template<class E>
struct sin_expression : expression<sin_expression<E> > {
    sin_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return std::sin(e(x));
    }
    const E e;
};
template<class E>
sin_expression<E> sin(const expression<E>& e) {
        return sin_expression<E>(e);
}

template<class E>
struct cos_expression : expression<cos_expression<E> > {
    cos_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return std::cos(e(x));
    }
    const E e;
};
template<class E>
cos_expression<E> cos(const expression<E>& e) {
    return cos_expression<E>(e);
}

template<class E>
struct tg_expression : expression<tg_expression<E> > {
    tg_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return std::tan(e(x));
    }
    const E e;
};
template<class E>
tg_expression<E> tan(const expression<E>& e) {
    return tg_expression<E>(e);
}

template<class E>
struct ctg_expression : expression<ctg_expression<E> > {
    ctg_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return 1 / std::tan(e(x));
    }
    const E e;
};
template<class E>
ctg_expression<E> ctan(const expression<E>& e) {
    return ctg_expression<E>(e);
}
// Exponent (exp, log)
template<class E>
struct exp_expression : expression<exp_expression<E> > {
    exp_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return std::exp(e(x));
    }
    const E e;
};
template<class E>
exp_expression<E> exp(const expression<E>& e) {
    return exp_expression<E>(e);
}

template<class E>
struct log_expression : expression<log_expression<E> > {
    log_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        //natural log (base = e ~ 2.72)
        return std::log(e(x));
    }
    const E e;
};
template<class E>
log_expression<E> log(const expression<E>& e) {
    return log_expression<E>(e);
}

// Other (sqrt, sqr, sign, abs, pow)
template<class E>
struct sqrt_expression : expression<sqrt_expression<E> > {
    sqrt_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return std::sqrt(e(x));
    }
    const E e;
};
template<class E>
sqrt_expression<E> sqrt(const expression<E>& e) {
    return sqrt_expression<E>(e);
}

template<class E>
struct sqr_expression : expression<sqr_expression<E> > {
    sqr_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return e(x) * e(x);
    }
    const E e;
};
template<class E>
sqr_expression<E> sqr(const expression<E>& e) {
    return sqr_expression<E>(e);
}

template<class E>
struct sign_expression : expression<sign_expression<E> > {
    sign_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return (e(x) > 0) ? 1 : ((e(x) < 0) ? -1 : 0);
    }
    const E e;
};
template<class E>
sign_expression<E> sign(const expression<E>& e) {
    return sign_expression<E>(e);
}

template<class E>
struct abs_expression : expression<abs_expression<E> > {
    abs_expression(const expression<E>& e) : e(e.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return std::abs(e(x));
    }
    const E e;
};
template<class E>
abs_expression<E> abs(const expression<E>& e) {
    return abs_expression<E>(e);
}

template<class E1, class E2>
struct pow_expression : expression<pow_expression<E1, E2> > {
    pow_expression(const expression<E1>& e1, const expression<E2>& e2) : e1(e1.self()), e2(e2.self()) {}
    template <typename T, std::size_t M>
    T operator()(const std::array<T, M>& x) const {
        return std::pow(e1(x), e2(x));
    }
    const E1 e1;
    const E2 e2;
};
template<class E1, class E2>
pow_expression<E1, E2> pow(const expression<E1>& e1, const expression<E2>& e2) {
    return pow_expression<E1, E2>(e1, e2);
}

}
// -----------------------------------------------------------

