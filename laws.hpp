#include "ops.hpp"
#include "ceval.hpp"

namespace propcalc {
    using A = ceval::Any<0>;
    using B = ceval::Any<1>;
    using C = ceval::Any<2>;
    bool IsMP(runtime::Node* A, runtime::Node* AtoB, runtime::Node* B) {
        if (!AtoB->Is<cexpr::Implies>()) {
            return false;
        }
        auto& atob = AtoB->As<cexpr::Implies>();
        return ops::SyntacticEq(A, atob[0]) && ops::SyntacticEq(atob[1], B);
    }
    using namespace ceval;
    bool ax1(runtime::Node* _) {
        return Implies<A, Implies<B, A>>::Match(_);
    }
    bool ax2(runtime::Node* _) {
        return Implies<
                    Implies<A, Implies<B, C>>,
                    Implies<Implies<A, B>, Implies<A, C>>
                >::Match(_);
    }
    bool ax3(runtime::Node* _) {
        return Implies<And<A, B>, A>:: Match(_);
    }
    bool ax4(runtime::Node* _) {
        return Implies<And<A, B>, B>:: Match(_);
    }
    bool ax5(runtime::Node* _) {
        return Implies<A, Implies<B, And<A,B>>>::Match(_);
    }
    bool ax6(runtime::Node* _) {
        return Implies<A, Or<A, B>>::Match(_);
    }
    bool ax7(runtime::Node* _) {
        return Implies<B, Or<A, B>>::Match(_);
    }
    bool ax8(runtime::Node* _) {
        return Implies< 
                Implies<A, C>, 
                Implies<
                    Implies<B, C>, 
                    Implies<Or<A, B>, C>
                >
            >::Match(_);
    }
    bool ax9(runtime::Node* _) {
        return Implies<Not<A>, Implies<A, B>>::Match(_);
    }
    bool ax10(runtime::Node* _) {
        return Implies<
                Implies<A, B>, 
                Implies<
                    Implies<A, Not<B>>,
                    Not<A>
                >
            >::Match(_);
    }
    bool ax11(runtime::Node* _) {
        return Or<A, Not<A>>::Match(_);
    }
};