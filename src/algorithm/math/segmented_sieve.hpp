#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace math{

long long segmented_sieve_isqrt(long long n){
    long long x = (long long)__builtin_sqrt((long double)n);
    while((__int128)(x + 1) * (x + 1) <= n){
        x++;
    }
    while((__int128)x * x > n){
        x--;
    }
    return x;
}

std::vector<char> segmented_sieve(long long l, long long r){
    if(l < 0 || l > r)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (segmented_sieve).");
    }
    std::vector<char> is_prime((size_t)(r - l), true);
    if(l == r){
        return is_prime;
    }

    const long long limit = r <= 1 ? 1 : segmented_sieve_isqrt(r - 1) + 1;
    std::vector<char> small((size_t)limit + 1, true);
    small[0] = false;
    if(limit >= 1){
        small[1] = false;
    }

    for(long long p = 2; p <= limit; p++){
        if(!small[(size_t)p]){
            continue;
        }
        if((__int128)p * p <= limit){
            for(long long q = p * p; q <= limit; q += p){
                small[(size_t)q] = false;
            }
        }

        __int128 start = std::max<__int128>((__int128)p * p, ((l + p - 1) / p) * (__int128)p);
        for(; start < r; start += p){
            is_prime[(size_t)(start - l)] = false;
        }
    }

    if(l == 0){
        is_prime[0] = false;
        if(r >= 2){
            is_prime[1] = false;
        }
    }else if(l == 1){
        is_prime[0] = false;
    }
    return is_prime;
}

std::vector<long long> primes_in_range(long long l, long long r){
    const auto is_prime = segmented_sieve(l, r);
    std::vector<long long> res;
    for(long long i = 0; i < (long long)is_prime.size(); i++){
        if(is_prime[(size_t)i]){
            res.push_back(l + i);
        }
    }
    return res;
}

} // namespace math
