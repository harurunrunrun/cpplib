// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/geometry_absolute_sign.hpp"
#include "../../src/algorithm/geometry/2d/geometry_scaled_sign.hpp"
#include "../../src/algorithm/geometry/2d/geometry_sign.hpp"
#include "geometry_2d_test_common.hpp"

#include <cassert>

int main(){
    assert(geometry_sign(2.0L * GEOMETRY_EPS) == 1);
    assert(geometry_sign(-2.0L * GEOMETRY_EPS) == -1);
    assert(geometry_sign(GEOMETRY_EPS / 2.0L) == 0);
    assert(geometry_scaled_sign(2.0L * GEOMETRY_EPS, 1.0L) == 1);
    assert(geometry_scaled_sign(-2.0L * GEOMETRY_EPS, 1.0L) == -1);
    assert(geometry_scaled_sign(GEOMETRY_EPS / 2.0L, 1.0L) == 0);
    assert(geometry_scaled_sign(1.0L, 0.0L, 0.0L) == 1);

    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double value;
        std::cin >> value;
        std::cout << geometry_sign(value) << '\n';
    }
}
