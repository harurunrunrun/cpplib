#pragma once

#include <stdexcept>
#include <utility>

namespace math{

template<class T>
T arithmetic_progression_sum(T first, T diff, long long n){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (arithmetic_progression_sum).");
    }
    T pairs;
    if(n % 2 == 0){
        pairs = T(n / 2) * T(n - 1);
    }else{
        pairs = T(n) * T((n - 1) / 2);
    }
    return first * T(n) + diff * pairs;
}

template<class T>
std::pair<T, T> geometric_series_pow_sum(T ratio, long long n){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (geometric_series_pow_sum).");
    }
    if(n == 0){
        return {T(1), T(0)};
    }
    auto [p, s] = geometric_series_pow_sum(ratio, n / 2);
    const T pp = p * p;
    const T ss = s * (T(1) + p);
    if(n % 2 == 0){
        return {pp, ss};
    }
    return {pp * ratio, ss + pp};
}

template<class T>
T geometric_series_sum(T ratio, long long n){
    return geometric_series_pow_sum(ratio, n).second;
}

template<class T>
T geometric_progression_sum(T first, T ratio, long long n){
    return first * geometric_series_sum(ratio, n);
}

} // namespace math
