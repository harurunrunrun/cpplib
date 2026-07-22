#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_DISCRETE_LOG_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_DISCRETE_LOG_HPP_INCLUDED

#include <stdexcept>
#include <unordered_map>

#include "basic_number_theory.hpp"

namespace math{

long long discrete_log_coprime(long long a, long long b, long long mod){
    if(mod <= 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (discrete_log_coprime).");
    }
    if(mod == 1){
        return 0;
    }
    a %= mod;
    b %= mod;
    if(a < 0){
        a += mod;
    }
    if(b < 0){
        b += mod;
    }
    if(b == 1 % mod){
        return 0;
    }
    if(gcd(a, mod) != 1)[[unlikely]]{
        throw std::runtime_error("library assertion fault: gcd is not 1 (discrete_log_coprime).");
    }

    long long n = isqrt(mod);
    if((__int128)n * n < mod){
        n++;
    }

    std::unordered_map<long long, long long> baby;
    baby.reserve((std::size_t)n * 2);
    long long cur = 1 % mod;
    for(long long q = 0; q < n; q++){
        baby.emplace(cur, q);
        cur = (long long)((__int128)cur * a % mod);
    }

    const long long step = inv_mod(pow_mod(a, n, mod), mod);
    cur = b;
    for(long long p = 0; p <= n; p++){
        const auto it = baby.find(cur);
        if(it != baby.end()){
            return p * n + it->second;
        }
        cur = (long long)((__int128)cur * step % mod);
    }
    return -1;
}

long long discrete_log(long long a, long long b, long long mod){
    if(mod <= 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (discrete_log).");
    }
    if(mod == 1){
        return 0;
    }
    a %= mod;
    b %= mod;
    if(a < 0){
        a += mod;
    }
    if(b < 0){
        b += mod;
    }
    if(b == 1 % mod){
        return 0;
    }

    long long coefficient = 1 % mod;
    long long shift = 0;
    long long g;
    while((g = gcd(a, mod)) > 1){
        if(b == coefficient){
            return shift;
        }
        if(b % g != 0){
            return -1;
        }
        b /= g;
        mod /= g;
        coefficient = (long long)((__int128)coefficient * (a / g) % mod);
        shift++;
    }

    const long long rhs = (long long)((__int128)b * inv_mod(coefficient, mod) % mod);
    const long long res = discrete_log_coprime(a, rhs, mod);
    if(res == -1){
        return -1;
    }
    return res + shift;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_DISCRETE_LOG_HPP_INCLUDED
