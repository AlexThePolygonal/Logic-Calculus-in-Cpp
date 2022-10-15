#pragma once
#include "runtime.hpp"

namespace runtime {

    template <size_t N>
    struct CevalInt {
        static size_t Eval() { return N;}
    };


    template <class Cur, class ... Children>
    struct Ceval {
        static Dyn<Cur>* Eval() {
            return new Dyn<Cur>((Children::Eval())...);
        }
        static bool VerifyTyping(runtime::Node* node) {
            if constexpr (std::is_same_v<Cur, cexpr::Anything>) {
                return true;
            }
            if (!node->Is<Cur>()) {
                return false;
            }
            auto& cur = node->As<Cur>();
            unsigned int i = 0;
            return (Children::VerifyTyping(cur[i++]) &&... );
        }
    };
    template <class Cur, size_t N,class ... Children>
    struct Ceval<Cur, CevalInt<N>, Children...> {
        static Dyn<Cur>* Eval() {
            return new Dyn<Cur>(N, (Children::Eval())...);
        }
        static bool VerifyTyping(runtime::Node* node) {
            if constexpr (std::is_same_v<Cur, cexpr::Anything>) {
                return true;
            }
            if (!node->Is<Cur>()) {
                return false;
            }
            auto& cur = node->As<Cur>();
            unsigned int i = 0;
            return (Children::VerifyTyping(cur[i++]) &&... );
        }
    };

};
#define FORM_CEVAL(T) template <class ... Args> using T = runtime::Ceval<cexpr::T, Args...>;
#define FORM_CEVAL_WITH_INT(T) template <size_t N = 0, class ... Args> using T = runtime::Ceval<cexpr::T, runtime::CevalInt<N>, Args...>;

namespace ceval {
    FORM_CEVAL_WITH_INT(Anything)
    FORM_CEVAL(Implies)
    FORM_CEVAL(And)
    FORM_CEVAL(Not)
    FORM_CEVAL(Or)
};