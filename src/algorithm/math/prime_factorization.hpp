#pragma once

#include <algorithm>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace math{

using u64 = unsigned long long;
using u128 = __uint128_t;

inline u64 mul_mod_u64(u64 a, u64 b, u64 mod){
    return (u64)((u128)a * b % mod);
}

inline u64 pow_mod_u64(u64 x, u64 n, u64 mod){
    u64 res = 1 % mod;
    while(n > 0){
        if(n & 1){
            res = mul_mod_u64(res, x, mod);
        }
        x = mul_mod_u64(x, x, mod);
        n >>= 1;
    }
    return res;
}

inline bool miller_rabin_test(u64 n, u64 a, u64 d, int s){
    u64 x = pow_mod_u64(a % n, d, n);
    if(x == 1 || x == n - 1){
        return true;
    }
    for(int i = 1; i < s; i++){
        x = mul_mod_u64(x, x, n);
        if(x == n - 1){
            return true;
        }
    }
    return false;
}

inline bool is_prime_miller_rabin(u64 n){
    if(n < 2){
        return false;
    }
    for(u64 p: {2ULL, 3ULL, 5ULL, 7ULL, 11ULL, 13ULL, 17ULL, 19ULL, 23ULL, 29ULL, 31ULL, 37ULL}){
        if(n % p == 0){
            return n == p;
        }
    }

    u64 d = n - 1;
    const int s = __builtin_ctzll(d);
    d >>= s;

    for(u64 a: {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL}){
        if(a % n == 0){
            continue;
        }
        if(!miller_rabin_test(n, a, d, s)){
            return false;
        }
    }
    return true;
}

template<class URBG>
bool is_prime_miller_rabin_random(u64 n, int rounds, URBG& rng){
    if(rounds < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (is_prime_miller_rabin_random).");
    }
    if(n < 2){
        return false;
    }
    for(u64 p: {2ULL, 3ULL, 5ULL, 7ULL, 11ULL, 13ULL, 17ULL, 19ULL, 23ULL, 29ULL, 31ULL, 37ULL}){
        if(n % p == 0){
            return n == p;
        }
    }

    u64 d = n - 1;
    const int s = __builtin_ctzll(d);
    d >>= s;

    std::uniform_int_distribution<u64> dist(2, n - 2);
    for(int i = 0; i < rounds; i++){
        if(!miller_rabin_test(n, dist(rng), d, s)){
            return false;
        }
    }
    return true;
}

inline u64 pollard_rho(u64 n){
    if(n <= 1)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (pollard_rho).");
    }
    if(n % 2 == 0){
        return 2;
    }
    if(n % 3 == 0){
        return 3;
    }
    if(is_prime_miller_rabin(n)){
        return n;
    }

    std::mt19937_64 rng(0x9e3779b97f4a7c15ULL ^ n);
    std::uniform_int_distribution<u64> dist(2, n - 2);

    while(true){
        const u64 c = dist(rng);
        u64 y = dist(rng);
        u64 r = 1;
        u64 q = 1;
        u64 g = 1;
        u64 x = 0;
        u64 ys = 0;

        auto f = [&](u64 v){
            return (mul_mod_u64(v, v, n) + c) % n;
        };

        constexpr u64 m = 128;
        while(g == 1){
            x = y;
            for(u64 i = 0; i < r; i++){
                y = f(y);
            }
            for(u64 k = 0; k < r && g == 1; k += m){
                ys = y;
                const u64 lim = std::min(m, r - k);
                for(u64 i = 0; i < lim; i++){
                    y = f(y);
                    const u64 diff = x > y ? x - y : y - x;
                    q = mul_mod_u64(q, diff, n);
                }
                g = std::gcd(q, n);
            }
            r <<= 1;
        }
        if(g == n){
            do{
                ys = f(ys);
                const u64 diff = x > ys ? x - ys : ys - x;
                g = std::gcd(diff, n);
            }while(g == 1);
        }
        if(g != n){
            return g;
        }
    }
}

inline void factorize_pollard_rho_rec(u64 n, std::vector<u64>& factors){
    if(n == 1){
        return;
    }
    if(is_prime_miller_rabin(n)){
        factors.push_back(n);
        return;
    }
    const u64 d = pollard_rho(n);
    factorize_pollard_rho_rec(d, factors);
    factorize_pollard_rho_rec(n / d, factors);
}

inline std::vector<std::pair<u64,int>> factorize_pollard_rho(u64 n){
    if(n == 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (factorize_pollard_rho).");
    }
    std::vector<u64> factors;
    factorize_pollard_rho_rec(n, factors);
    std::sort(factors.begin(), factors.end());

    std::vector<std::pair<u64,int>> res;
    for(u64 p: factors){
        if(res.empty() || res.back().first != p){
            res.push_back({p, 1});
        }else{
            res.back().second++;
        }
    }
    return res;
}

inline u64 euler_phi(u64 n){
    if(n == 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (euler_phi).");
    }
    u64 res = n;
    for(auto [p, e]: factorize_pollard_rho(n)){
        (void)e;
        res = res / p * (p - 1);
    }
    return res;
}

} // namespace math
