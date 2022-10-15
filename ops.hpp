#pragma once
#include "runtime.hpp"
#include <set>


namespace ops {

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
        case Id<Anything>: return UnfoldIntoF::AtIdx<F, Anything, Res, &Recurse<F, Res>, 0>(f, &cur_node->As<Anything>()); 
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
        static_assert(std::is_base_of_v<cexpr::HasIntValBase, T>);
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

    runtime::Node* Clone(runtime::Node* _) {
        auto cloner = [](auto&& v, auto&&...args) -> runtime::Node* {
            return runtime::Clone(std::forward<std::remove_reference_t<decltype(v)>>(v),
            std::forward<std::remove_reference_t<decltype(args)>>(args) ...);
        };
        return Recurse<decltype(cloner), runtime::Node*>(cloner, _);
    }
}
