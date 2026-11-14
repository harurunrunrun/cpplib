// competitive-verifier: STANDALONE

#include <cassert>
#include <climits>
#include <cstdint>
#include <stdexcept>

#include "../../src/algorithm/math/number_theory/prime/smallest_prime_factor_prefix_sum.hpp"

int main(){
    const math::SmallestPrimeFactorPrefixSum table(200);
    assert(table.limit() == 200);
    std::uint64_t expected = 0;
    for(int n = 0; n <= 200; ++n){
        if(n >= 2){
            int factor = 2;
            while(n % factor != 0) ++factor;
            expected += static_cast<std::uint64_t>(factor);
        }
        assert(table.value(n) == expected);
    }
    const math::SmallestPrimeFactorPrefixSum empty(0);
    assert(empty.limit() == 0 && empty.value(0) == 0);
    for(const int limit: {-1, -2, INT_MIN}){
        bool thrown = false;
        try{
            const math::SmallestPrimeFactorPrefixSum invalid(limit);
            (void)invalid;
        }catch(const std::invalid_argument&){
            thrown = true;
        }
        assert(thrown);
    }
}
