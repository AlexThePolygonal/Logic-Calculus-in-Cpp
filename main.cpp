#include "cexpr.hpp"
#include <iostream>
#include "cexpr.hpp"
#include "runtime.hpp"
#include "ceval.hpp"
#include <memory>
#include "ops.hpp"
#include "laws.hpp"

#define ASSERT(cond) if (!(cond)) {return false;}
#define MAKE_TEST(name, body) bool TEST_##name () {{body} return true;}
#define RUN_TEST(name) { std::cout << (TEST_##name () ? std::string("TEST ") + #name + " passed! :)\n" : std::string("TEST ") + #name + " FAILED! :/\n"); }

MAKE_TEST(ListTypeids,
    std::cout << 
    "cexpr::Id<cexpr::Any> = "     << cexpr::Id<cexpr::Any>     << '\n' <<
    "cexpr::Id<cexpr::Implies> = " << cexpr::Id<cexpr::Implies> << '\n' <<
    "cexpr::Id<cexpr::And> = "     << cexpr::Id<cexpr::And>     << '\n' <<
    "cexpr::Id<cexpr::Or> = "      << cexpr::Id<cexpr::Or>      << '\n' <<
    "cexpr::Id<cexpr::Not> = "     << cexpr::Id<cexpr::Not>     << '\n' <<
    '\n';
    std::set<unsigned> set;
    set.insert(cexpr::Id<cexpr::Any>);
    set.insert(cexpr::Id<cexpr::Implies>);
    set.insert(cexpr::Id<cexpr::And>);
    set.insert(cexpr::Id<cexpr::Or>);
    set.insert(cexpr::Id<cexpr::Not>);
    ASSERT(set.size() == 5);
)
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
        runtime::Node* ptr = ceval::Any<3>::Eval();
        ASSERT(ptr->Is<cexpr::Any>() == true);
        ASSERT(ptr->As<cexpr::Any>().val == 3)
        delete ptr;
    } 
    {   
        runtime::Node* ptr;
        {
        using namespace ceval;
        ptr = (Implies<Any<0>,Any<0>>::Eval());
        }
        {
        using namespace cexpr;
        ASSERT(ptr->Is<Implies>());
        auto& val = ptr->As<Implies>();
        ASSERT(val[0]->Is<Any>());
        ASSERT(val[1]->Is<Any>());
        ASSERT((val[0]->As<Any>().val == 0))
        ASSERT((val[1]->As<Any>().val == 0))
        }
        delete ptr;
    }
);
MAKE_TEST(EasyCevalVerify,
    {   
        runtime::Node* ptr;
        using namespace ceval;
        ptr = (Implies<Implies<Any<0>,Any<0>>,Any<0>>::Eval());
        ASSERT((Implies<Any<>,Any<>>::VerifyTyping(ptr)))
        delete ptr;
    }
);

MAKE_TEST(EasyRecurse,
    {
        runtime::Node* ptr;
        using namespace ceval;
        ptr = (Implies<Implies<Any<0>, Any<1>>, Any<2>>::Eval());
        auto res = ops::ListIds<cexpr::Any>(ptr);
        ASSERT(res.size() == 3)
        ASSERT(res.contains(0))
        ASSERT(res.contains(1))
        ASSERT(res.contains(2))
        delete ptr;
    }
    {
        runtime::Node* ptr;
        using namespace ceval;
        ptr = (Implies<Implies<Any<0>, Any<1>>, Any<1>>::Eval());
        auto res = ops::ListIds<cexpr::Any>(ptr);
        ASSERT(res.size() == 2)
        ASSERT(res.contains(0))
        ASSERT(res.contains(1))
        delete ptr;
    }
    {
        runtime::Node* ptr;
        { using namespace ceval;
        ptr = (Implies<Implies<Any<0>, Any<1>>, Any<1>>::Eval());
        }
        runtime::Node* ptr1 = ops::Clone(ptr);
        ASSERT(ops::SyntacticEq(ptr, ptr1));
        delete ptr;
        { using namespace cexpr;
        ASSERT(ptr1->Is<Implies>());
        auto& impl1 = ptr1->As<Implies>();
        ASSERT(impl1[0]->Is<Implies>());
        ASSERT(impl1[1]->Is<Any>());
        ASSERT(impl1[1]->As<Any>().val == 1);
        ASSERT(impl1[0]->As<Implies>()[0]->As<Any>().val == 0);
        ASSERT(impl1[0]->As<Implies>()[1]->As<Any>().val == 1);
        }
        delete ptr1;
    }
)
MAKE_TEST(EasyPatternmatch,
    using namespace ceval;
    runtime::Node* ptr = (And<And<Any<0>, Any<1>>, And<Any<0>, Any<1>>>::Eval());
    ASSERT((And<Any<0>, Any<1>>::Match(ptr)));
    ASSERT((And<Any<0>, Any<0>>::Match(ptr)));
    delete ptr;
)


int main() {
    std::cout << std::boolalpha;
    RUN_TEST(ListTypeids)
    RUN_TEST(EasyIs)
    RUN_TEST(EasyCevalEval)
    RUN_TEST(EasyCevalVerify)
    RUN_TEST(EasyRecurse)
    RUN_TEST(EasyPatternmatch)

}

