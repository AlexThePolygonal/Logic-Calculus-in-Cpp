#pragma once
#include "cexpr.hpp"
#include "cexpr_counter.hpp"
#include <optional>
#include <stack>
#include "n_ary.hpp"

//////////////////////////
//  INTENTIONALLY CRUDE //
//////////////////////////

namespace runtime {



    namespace detail {
    template <class T> constexpr auto GetId() { return aux::next();}
    template <class T, auto = [](){}> struct RenewT { constexpr RenewT() = default; constexpr RenewT(int) {};};
    template <class T> auto GetDyn();
    };
    template <class T> using Dyn = std::remove_reference_t<decltype(*detail::GetDyn<T>())>;


    class Node {
    public:
        unsigned cur_type_id;

        Node(unsigned cur_type_) : cur_type_id(cur_type_) {}
        Node(Node&&) = default;

        template <class T> bool Is() const noexcept { return Dyn<T>::type_id == cur_type_id; }
        template <class T> Dyn<T>& As() noexcept { return static_cast<Dyn<T>&>(*this); }
        template <class T> const Dyn<T>& As() const noexcept { return static_cast<const Dyn<T>&>(*this); }

        virtual Node** begin() noexcept = 0;
        virtual Node** end() noexcept = 0;

        virtual ~Node() {}
    };

    template <class T, class U>
    class DynImpl {};

    template <class T>
    class DynImpl<T, void> : public WithArity<DynImpl<T, void>, Node, T::arity> {
        using Parent = WithArity<DynImpl<T, void>, Node, T::arity>;
    public:
        using CexprT = T;
        constexpr static unsigned type_id = detail::GetId<DynImpl<T, void>>() - 1;
        template <class W> static constexpr bool CexprIs() { return type_id  == Dyn<W>::type_id; }


        template <class ... Args>
        DynImpl(Args&& ... args) : Parent(std::forward<Args>(args)...) {}


        virtual ~DynImpl() {};
    };
    
    template <class T>
    class DynImpl<T, typename T::IntegerType> : public WithArity<DynImpl<T, typename T::IntegerType>, Node, T::arity> {
        using Parent = WithArity<DynImpl<T, typename T::IntegerType>, Node, T::arity>;
    public:
        T::IntegerType val;

        using CexprT = T;
        constexpr static unsigned type_id = detail::GetId<DynImpl<T, typename T::IntegerType>>() - 1;
        template <class W> static constexpr bool CexprIs() { return type_id == Dyn<W>::type_id; }


        template <class U, class ... Args>
        DynImpl(U&& u, Args&& ... args) : Parent(std::forward<Args>(args)...) , val(std::forward<U>(u)){}

        virtual ~DynImpl() {};
    };

    template <class T> auto detail::GetDyn() {
        if constexpr (cexpr::has_integer_value_v<T>) {
            return static_cast<DynImpl<T, typename T::IntegerType>*>(nullptr);
        }
        if constexpr (!cexpr::has_integer_value_v<T>) {
            return static_cast<DynImpl<T, void>*>(nullptr);
        }
        // never called
    }

    template <class T, class ... Args>
    T* Clone(T* self, Args ... args) {
        if constexpr (cexpr::has_integer_value_v< typename T::CexprT>) {
            return new T(self->val, std::forward<Args>(args)...);
        }
        if constexpr (!cexpr::has_integer_value_v<typename T::CexprT>) {
            return new T(std::forward<Args>(args)...);
        }
    }

};


template <class T> constexpr unsigned cexpr::GetTypeid() { return runtime::Dyn<T>::type_id; };



