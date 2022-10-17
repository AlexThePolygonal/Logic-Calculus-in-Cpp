#pragma once
#include "runtime.hpp"
#include <set>
#include <map>


namespace ops {

    bool SyntacticEq(runtime::Node*, runtime::Node*);

    struct UnfoldIntoSyneq {
        template <unsigned arity, unsigned idx, class T>
        constexpr static inline bool AtIdx(runtime::Dyn<T>& l, runtime::Dyn<T>& r) {
            if constexpr (idx == arity) {
                return true;
            }
            if constexpr (idx < arity) {
                return SyntacticEq(l[idx],r[idx]) && AtIdx<arity, idx+1, T>(l,r);
            }
        }
    };



    template <class T>
    bool SyntacticEqImpl(runtime::Dyn<T>& l, runtime::Dyn<T>& r) {
        if constexpr (cexpr::has_integer_value_v<T>) {
            if (l.val != r.val) {
                return false;
            }
        }
        return UnfoldIntoSyneq::AtIdx<T::arity, 0, T>(l,r);
    }

    bool SyntacticEq(runtime::Node* l, runtime::Node* r) {
        if (l->cur_type_id != r->cur_type_id) {
            return false;
        }
        using namespace cexpr;
        switch (l->cur_type_id) {
        case Id<Any> :     return SyntacticEqImpl<Any>    (l->As<Any>(),     r->As<Any>());
        case Id<Implies> : return SyntacticEqImpl<Implies>(l->As<Implies>(), r->As<Implies>());
        case Id<And> :     return SyntacticEqImpl<And>    (l->As<And>(),     r->As<And>());
        case Id<Or> :      return SyntacticEqImpl<Or>     (l->As<Or>(),      r->As<Or>());
        case Id<Not> :     return SyntacticEqImpl<Not>    (l->As<Not>(),     r->As<Not>());
        default:  throw std::logic_error("ops::SyntacticEq error: unknown type id  " + std::to_string(l->cur_type_id));
        }
    }

    struct UnfoldIntoF {
        template <
            class F, class T, class Res, 
            Res (*Recurse)(F&, runtime::Node*),
            unsigned cur_idx, 
            class ... Args
        >
        static inline Res AtIdx(
            F& functor_to_recurse, 
            runtime::Dyn<T>* cur_node,
            Args&& ... current_args
        ) {
            if constexpr (cur_idx == T::arity) {
                return functor_to_recurse(cur_node, Recurse(functor_to_recurse, current_args)...);
            }
            if constexpr (cur_idx < T::arity) {
                return AtIdx<F, T, Res, Recurse, cur_idx + 1, Args..., runtime::Node*>(
                    functor_to_recurse, cur_node, std::forward<Args>(current_args)..., cur_node->operator[](cur_idx)
                );
            }
        }
    };
    template <class F, class Res>
    Res Recurse(F& f, runtime::Node* cur_node) {
        using namespace cexpr;

        switch (cur_node->cur_type_id) {
        case Id<Any>:      return UnfoldIntoF::AtIdx<F, Any,      Res, &Recurse<F, Res>, 0>(f, &cur_node->As<Any>()); 
        case Id<Implies>:  return UnfoldIntoF::AtIdx<F, Implies,  Res, &Recurse<F, Res>, 0>(f, &cur_node->As<Implies>());
        case Id<And>:      return UnfoldIntoF::AtIdx<F, And,      Res, &Recurse<F, Res>, 0>(f, &cur_node->As<And>());
        case Id<Or>:       return UnfoldIntoF::AtIdx<F, Or,       Res, &Recurse<F, Res>, 0>(f, &cur_node->As<Or>());
        case Id<Not>:      return UnfoldIntoF::AtIdx<F, Not,      Res, &Recurse<F, Res>, 0>(f, &cur_node->As<Not>());
        default:           throw std::logic_error("ops::Recurse error: unknown type id  " + std::to_string(cur_node->cur_type_id));
        }
    }

    template <class T, class W>
    constexpr bool IsExample = std::remove_reference_t<T>::type_id == cexpr::Id<W>;


    template <class T>
    std::set<unsigned> ListIds(runtime::Node* _) {
        static_assert(cexpr::has_integer_value_v<T>);
        auto lister = [](auto&& v, auto&&...args) -> std::set<unsigned> {
            std::set<unsigned> res;
            if constexpr (IsExample<decltype(*v), T>) {
                res.insert(v->val);
                return res;
            }
            (res.merge(std::forward<std::remove_reference_t<decltype(args)>>(args)), ...);
            return res;
        };
        return Recurse<decltype(lister), std::set<unsigned>>(lister, _);
    }

    template <class T>
    void RenameIds(runtime::Node* _, std::map<unsigned, unsigned> map) {
        static_assert(cexpr::has_integer_value_v<T>);
        auto renamer = [&map](auto&& v, auto&& ...) -> void {
            if constexpr (IsExample<decltype(*v), T>) {
                v->val = map[v->val];
            }
        };
        Recurse<decltype(renamer), void>(renamer, _);
    }

    template <class T>
    void SafeRenameIds(runtime::Node* _, std::map<unsigned, unsigned> map) {
        static_assert(cexpr::has_integer_value_v<T>);
        auto renamer = [&map](auto&& v, auto&& ...) -> void {
            if constexpr (IsExample<decltype(*v), T>) {
                v->val = map.at(v->val);
            }
        };
        Recurse<decltype(renamer), void>(renamer, _);
    } 

    runtime::Node* CloneImpl(runtime::Node* _) {
        auto cloner = [](auto&& v, auto&&...args) -> runtime::Node* {
            return runtime::Clone(std::forward<std::remove_reference_t<decltype(v)>>(v),
            std::forward<std::remove_reference_t<decltype(args)>>(args) ...);
        };
        return Recurse<decltype(cloner), runtime::Node*>(cloner, _);
    }

    template <class T> T* Clone(T* _) { return &(CloneImpl(_)-> template As<T>()); }
    template <> runtime::Node* Clone(runtime::Node* _) { return CloneImpl(_); }

}
