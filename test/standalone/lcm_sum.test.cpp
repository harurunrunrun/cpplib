// competitive-verifier: STANDALONE

#include <cassert>
#include <climits>
#include <cstdint>
#include <numeric>
#include <stdexcept>

#include "../../src/algorithm/math/number_theory/multiplicative/lcm_sum.hpp"

int main(){
    const math::LcmSumTable table(100);
    assert(table.limit() == 100);
    for(int n = 1; n <= 100; ++n){
        std::uint64_t expected = 0;
        for(int i = 1; i <= n; ++i){
            expected += static_cast<std::uint64_t>(std::lcm(i, n));
        }
        assert(table.value(n) == expected);
    }
    for(const int limit: {0, -1, -2, INT_MIN}){
        bool thrown = false;
        try{
            const math::LcmSumTable invalid(limit);
            (void)invalid;
        }catch(const std::invalid_argument&){
            thrown = true;
        }
        assert(thrown);
    }
}
