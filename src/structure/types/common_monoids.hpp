#pragma once

#include <algorithm>
#include <limits>
#include <numeric>
#include <type_traits>

template<class T>
struct AddMonoid{
    using S = T;

    constexpr S op(S a, S b)const{
        return a + b;
    }

    constexpr S e()const{
        return S(0);
    }
};

template<class T>
struct MulMonoid{
    using S = T;

    constexpr S op(S a, S b)const{
        return a * b;
    }

    constexpr S e()const{
        return S(1);
    }
};

template<class T>
struct MinMonoid{
    using S = T;

    T identity = std::numeric_limits<T>::max();

    constexpr S op(S a, S b)const{
        return std::min(a, b);
    }

    constexpr S e()const{
        return identity;
    }
};

template<class T>
struct MaxMonoid{
    using S = T;

    T identity = std::numeric_limits<T>::lowest();

    constexpr S op(S a, S b)const{
        return std::max(a, b);
    }

    constexpr S e()const{
        return identity;
    }
};

template<class T>
struct GcdMonoid{
    static_assert(std::is_integral_v<T>);

    using S = T;

    constexpr S op(S a, S b)const{
        return std::gcd(a, b);
    }

    constexpr S e()const{
        return S(0);
    }
};

template<class T>
struct LcmMonoid{
    static_assert(std::is_integral_v<T>);

    using S = T;

    constexpr S op(S a, S b)const{
        return std::lcm(a, b);
    }

    constexpr S e()const{
        return S(1);
    }
};

template<class T>
struct XorMonoid{
    static_assert(std::is_integral_v<T>);

    using S = T;

    constexpr S op(S a, S b)const{
        return a ^ b;
    }

    constexpr S e()const{
        return S(0);
    }
};

template<class T>
struct BitAndMonoid{
    static_assert(std::is_integral_v<T>);

    using S = T;

    constexpr S op(S a, S b)const{
        return a & b;
    }

    constexpr S e()const{
        return ~S(0);
    }
};

template<class T>
struct BitOrMonoid{
    static_assert(std::is_integral_v<T>);

    using S = T;

    constexpr S op(S a, S b)const{
        return a | b;
    }

    constexpr S e()const{
        return S(0);
    }
};
