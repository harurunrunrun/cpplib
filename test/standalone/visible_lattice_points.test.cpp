// competitive-verifier: STANDALONE

#include <cassert>
#include <climits>
#include <cstdint>
#include <numeric>
#include <stdexcept>

#include "../../src/algorithm/math/number_theory/multiplicative/visible_lattice_points.hpp"

int main(){
    const math::VisibleLatticePointsTable table(20);
    assert(table.limit() == 20);
    for(int n = 1; n <= 20; ++n){
        std::uint64_t expected = 0;
        for(int x = 0; x <= n; ++x){
            for(int y = 0; y <= n; ++y){
                for(int z = 0; z <= n; ++z){
                    if(std::gcd(std::gcd(x, y), z) == 1) ++expected;
                }
            }
        }
        assert(table.value(n) == expected);
    }
    for(const int limit: {0, -1, -2, INT_MIN}){
        bool thrown = false;
        try{
            const math::VisibleLatticePointsTable invalid(limit);
            (void)invalid;
        }catch(const std::invalid_argument&){
            thrown = true;
        }
        assert(thrown);
    }
}
