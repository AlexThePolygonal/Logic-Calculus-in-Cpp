#pragma once

namespace aux {
    template<class _,int>                                         
    struct Flag {
        friend constexpr bool flag(Flag);
    };

    template<class _,int N>
    struct Writer {
        friend constexpr bool flag(Flag<_,N>) { 
            return true; 
        }
        static constexpr int value = N;
    };

    template<class _,int N = 0>
    constexpr int reader(float) {
        return Writer<_,N>::value;
    }

    template<class _,int N = 0,
            bool = flag(Flag<_,N>{}),
            auto = [](){}>
    constexpr int reader(int) {
        return reader<_,N + 1>(int{});
    }

    template<class _,int R = reader<_,0>(int{})>
    constexpr int next() {
        return R;
    }
};