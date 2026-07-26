// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/geometry_absolute_sign.hpp"
#include "../../src/algorithm/geometry/2d/geometry_scaled_sign_value_tolerance_roundoff_scales.hpp"
#include "../../src/algorithm/geometry/2d/geometry_scaled_sign_value_scale.hpp"
#include "../../src/algorithm/geometry/2d/geometry_scaled_sign.hpp"
#include "../../src/algorithm/geometry/2d/geometry_sign.hpp"
#include "geometry_2d_test_common.hpp"

#include <cassert>
#include <cmath>
#include <limits>

int main(){
    assert(geometry_sign(2.0L * GEOMETRY_EPS) == 1);
    assert(geometry_sign(-2.0L * GEOMETRY_EPS) == -1);
    assert(geometry_sign(GEOMETRY_EPS / 2.0L) == 0);
    assert(geometry_scaled_sign(2.0L * GEOMETRY_EPS, 1.0L) == 1);
    assert(geometry_scaled_sign(-2.0L * GEOMETRY_EPS, 1.0L) == -1);
    assert(geometry_scaled_sign(GEOMETRY_EPS / 2.0L, 1.0L) == 0);
    assert(geometry_scaled_sign(1.0L, 0.0L, 0.0L) == 1);
    const long double machine_epsilon =
        std::numeric_limits<long double>::epsilon();
    const long double split_tolerance =
        GEOMETRY_EPS * 2.0L + 64.0L * machine_epsilon * 3.0L;
    assert(geometry_scaled_sign(split_tolerance, -2.0L, -3.0L) == 0);
    assert(geometry_scaled_sign(std::nextafter(
        split_tolerance, std::numeric_limits<long double>::infinity()
    ), 2.0L, 3.0L) == 1);
    assert(geometry_scaled_sign(std::nextafter(
        -split_tolerance, -std::numeric_limits<long double>::infinity()
    ), 2.0L, 3.0L) == -1);
    const long double shared_tolerance =
        GEOMETRY_EPS + 64.0L * machine_epsilon;
    assert(geometry_scaled_sign(shared_tolerance, -1.0L) == 0);
    assert(geometry_scaled_sign(std::nextafter(
        shared_tolerance, std::numeric_limits<long double>::infinity()
    ), 1.0L) == 1);
    assert(geometry_scaled_sign(std::nextafter(
        -shared_tolerance, -std::numeric_limits<long double>::infinity()
    ), 1.0L) == -1);

    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double value;
        std::cin >> value;
        std::cout << geometry_sign(value) << '\n';
    }
}
