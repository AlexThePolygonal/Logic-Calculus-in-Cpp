#include "cexpr.hpp"
#include <iostream>
#include "cexpr.hpp"
#include "runtime.hpp"
#include "ceval.hpp"
#include <memory>
#include "ops.hpp"

#define ASSERT(cond) if (!(cond)) {return false;}
#define MAKE_TEST(name, body) bool TEST_##name () {{body} return true;}
#define RUN_TEST(name) { std::cout << (TEST_##name () ? std::string("TEST ") + #name + " passed! :)\n" : std::string("TEST ") + #name + " FAILED! :/\n"); }

MAKE_TEST(EasyIs, 
    {
    runtime::Node* ptr = new runtime::Dyn<cexpr::Implies>(nullptr, nullptr);
    delete ptr;
    ptr = new runtime::Dyn<cexpr::And>(nullptr, nullptr);
    ASSERT(ptr->Is<cexpr::Implies>() == false);
    ASSERT(ptr->Is<cexpr::And>() == true);
    delete ptr;
    }
)
MAKE_TEST(EasyCevalEval,
    {
        runtime::Node* ptr = ceval::Anything<3>::Eval();
        ASSERT(ptr->Is<cexpr::Anything>() == true);
        ASSERT(ptr->As<cexpr::Anything>().val == 3)
        delete ptr;
    } 
    {   
        runtime::Node* ptr;
        {
        using namespace ceval;
        ptr = (Implies<Anything<0>,Anything<0>>::Eval());
        }
        {
        using namespace cexpr;
        ASSERT(ptr->Is<Implies>());
        auto& val = ptr->As<Implies>();
        ASSERT(val[0]->Is<Anything>());
        ASSERT(val[1]->Is<Anything>());
        ASSERT((val[0]->As<Anything>().val == 0))
        ASSERT((val[1]->As<Anything>().val == 0))
        }
        delete ptr;
    }
);
MAKE_TEST(EasyCevalVerify,
    {   
        runtime::Node* ptr;
        using namespace ceval;
        ptr = (Implies<Implies<Anything<0>,Anything<0>>,Anything<0>>::Eval());
        ASSERT((Implies<Anything<>,Anything<>>::VerifyTyping(ptr)))
        delete ptr;
    }
);

MAKE_TEST(EasyRecurse,
    {
        runtime::Node* ptr;
        using namespace ceval;
        ptr = (Implies<Implies<Anything<0>, Anything<1>>, Anything<2>>::Eval());
        auto res = ops::ListIds<cexpr::Anything>(ptr);
        ASSERT(res.size() == 3)
        ASSERT(res.contains(0))
        ASSERT(res.contains(1))
        ASSERT(res.contains(2))
        delete ptr;
    }
    {
        runtime::Node* ptr;
        using namespace ceval;
        ptr = (Implies<Implies<Anything<0>, Anything<1>>, Anything<1>>::Eval());
        auto res = ops::ListIds<cexpr::Anything>(ptr);
        ASSERT(res.size() == 2)
        ASSERT(res.contains(0))
        ASSERT(res.contains(1))
        delete ptr;
    }
)


int main() {
    std::cout << std::boolalpha;
    RUN_TEST(EasyIs)
    RUN_TEST(EasyCevalEval)
    RUN_TEST(EasyCevalVerify)
    RUN_TEST(EasyRecurse)

}
