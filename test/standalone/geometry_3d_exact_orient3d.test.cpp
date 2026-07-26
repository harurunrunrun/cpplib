// competitive-verifier: STANDALONE

#include <cstdint>
#include <limits>

#include "../../src/algorithm/geometry/3d/exact_orient3d.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const IntegerPoint3 a{0, 0, 0}, b{1, 0, 0};
        const IntegerPoint3 c{0, 1, 0}, d{0, 0, 1};
        if(exact_orient3d(a, b, c, d) != 1) return false;
        if(exact_orient3d(a, c, b, d) != -1) return false;
        if(exact_orient3d(a, b, c, {1, 1, 0}) != 0) return false;
        constexpr std::int64_t low = std::numeric_limits<std::int64_t>::min();
        constexpr std::int64_t high = std::numeric_limits<std::int64_t>::max();
        if(exact_orient3d(
            {low, low, low}, {high, low, low},
            {low, high, low}, {low, low, high}
        ) != 1) return false;
        return true;
    });
}
