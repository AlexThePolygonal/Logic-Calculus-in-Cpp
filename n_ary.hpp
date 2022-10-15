#pragma once
#include <array>
#include <cstddef>

template <class T, class Metatype, size_t arity, class ChildMetatype = Metatype>
class WithArity : public Metatype {
    std::array<ChildMetatype*, arity> arr;
public:
    constexpr static size_t node_arity = arity;

    auto& AsArr() noexcept { return arr;}
    virtual ChildMetatype** begin() noexcept final { return arr.begin();};
    virtual ChildMetatype** end() noexcept final { return arr.end();};
    
    const ChildMetatype* operator[](size_t i) const noexcept { return arr[i]; }
    ChildMetatype* operator[](size_t i) noexcept { return arr[i];}



    template <class ... Args>
    WithArity(Args&& ... args) : Metatype(T::type_id), arr({std::forward<Args>(args)...}) {}


    ~WithArity() {
        for (ChildMetatype* ptr : arr) {
            delete ptr;
        }
    }
};


