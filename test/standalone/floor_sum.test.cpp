// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "../../src/algorithm/math/number_theory/floor_sum.hpp"

long long floor_sum_naive(long long n, long long m, long long a, long long b){
    __int128 answer = 0;
    for(long long i = 0; i < n; ++i){
        const __int128 value = static_cast<__int128>(a) * i + b;
        __int128 quotient = value / m;
        if(value % m < 0) --quotient;
        answer += quotient;
    }
    return static_cast<long long>(answer);
}

int main(){
    int query_count;
    if(std::cin >> query_count){
        while(query_count--){
            long long n, m, a, b;
            std::cin >> n >> m >> a >> b;
            std::cout << math::floor_sum(n, m, a, b) << '\n';
        }
        return 0;
    }

    for(long long n = 0; n <= 20; ++n){
        for(long long m = 1; m <= 20; ++m){
            for(long long a = -20; a <= 20; ++a){
                for(long long b = -20; b <= 20; ++b){
                    assert(math::floor_sum(n, m, a, b) == floor_sum_naive(n, m, a, b));
                }
            }
        }
    }
    assert(math::floor_sum(
        std::numeric_limits<long long>::max(),
        std::numeric_limits<long long>::max(),
        -1,
        std::numeric_limits<long long>::max() - 1
    ) == 0);

    bool thrown = false;
    try{
        (void)math::floor_sum(-1, 1, 0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)math::floor_sum(1, 0, 0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)math::floor_sum(
            std::numeric_limits<long long>::max(),
            1,
            std::numeric_limits<long long>::max(),
            0
        );
    }catch(const std::overflow_error&){
        thrown = true;
    }
    assert(thrown);
}
