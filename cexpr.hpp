#pragma once
#include <cstddef>
#include <type_traits>
#include "cexpr_counter.hpp"
#include <gmpxx.h>

namespace cexpr {

    // arity;
    struct ArityDefined{};
    template <size_t N> struct Arity : ArityDefined { constexpr static size_t arity = N; };
    template <class T> constexpr bool IsArityDefined = std::is_base_of<ArityDefined, T>::value;

    struct HasIntValBase {};
    template <class IntegerType_ = mpz_class>
    struct HasIntVal : HasIntValBase { using IntegerType = IntegerType_; };
    template <class T> constexpr bool has_integer_value_v = std::is_base_of_v<HasIntValBase, T>;
    struct HasIdBase {};
    template <class T> struct HasId : HasIntVal<unsigned> { constexpr static unsigned idtype = aux::next<HasIdBase>() - 1;};
    template <class T> constexpr bool has_id_v = std::is_base_of_v<HasIdBase, T>;


    struct Any : Arity<0>, HasId<Any> {};
    
    struct Implies : Arity<2> {};
    struct Or : Arity<2> {};
    struct And : Arity<2> {};
    struct Not : Arity<1> {};

    struct AllTypes {};
    template <class T> constexpr unsigned GetTypeid() { return aux::next<AllTypes>() - 1;}
    template <class T> constexpr unsigned Id = GetTypeid<T>();

};


