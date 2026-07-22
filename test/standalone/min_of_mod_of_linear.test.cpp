// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/number_theory/min_of_mod_of_linear.hpp"

namespace{

using i128 = __int128_t;

long long brute(long long count, long long modulus, long long coefficient, long long constant){
    long long result = modulus;
    for(long long index = 0; index < count; ++index){
        result = std::min(
            result,
            static_cast<long long>(
                (static_cast<i128>(coefficient) * index + constant) % modulus
            )
        );
    }
    return result;
}

void self_test(){
    for(long long modulus = 1; modulus <= 50; ++modulus){
        for(long long count = 1; count <= 35; ++count){
            for(long long coefficient = 0; coefficient < modulus; ++coefficient){
                for(long long constant = 0; constant < modulus; ++constant){
                    assert(math::min_of_mod_of_linear(
                        count, modulus, coefficient, constant
                    ) == brute(count, modulus, coefficient, constant));
                }
            }
        }
    }
    assert(math::min_of_mod_of_linear(1'000'000'000, 1, 0, 0) == 0);

    bool thrown = false;
    try{
        (void)math::min_of_mod_of_linear(0, 7, 2, 3);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    self_test();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- != 0){
        long long count, modulus, coefficient, constant;
        std::cin >> count >> modulus >> coefficient >> constant;
        std::cout << math::min_of_mod_of_linear(
            count, modulus, coefficient, constant
        ) << '\n';
    }
}
