// competitive-verifier: STANDALONE

#include <cmath>
#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/orthogonal.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        if(!orthogonal({1, 2, 0}, {2, -1, 3}) ||
            orthogonal({1, 0, 0}, {1, 1, 0}) ||
            !orthogonal({}, {1, 2, 3})) return false;
        const long double denormal = std::numeric_limits<long double>::denorm_min();
        if(!orthogonal({denormal, 0, 0}, {0, denormal, 0})) return false;
        for(int first_exponent: {
            std::numeric_limits<long double>::min_exponent, -100, 100,
            std::numeric_limits<long double>::max_exponent - 3
        }){
            const long double first_scale = std::ldexp(1.0L, first_exponent);
            const long double second_scale = std::ldexp(
                1.0L, -first_exponent / 2
            );
            if(!orthogonal(
                {first_scale, 2 * first_scale, 0},
                {2 * second_scale, -second_scale, 3 * second_scale}
            )) return false;
        }
        for(const Point3 invalid_point: {
            Point3{std::numeric_limits<long double>::infinity(), 0, 0},
            Point3{std::numeric_limits<long double>::quiet_NaN(), 0, 0}
        }){
            bool invalid = false;
            try{
                (void)orthogonal(invalid_point, {1, 0, 0});
            }catch(const std::invalid_argument&){
                invalid = true;
            }
            if(!invalid) return false;
        }
        return true;
    });
}
