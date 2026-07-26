// competitive-verifier: STANDALONE

#include <cmath>
#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/coplanar.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        if(!coplanar(
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {2, 3, 0}
        ) || coplanar(
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
        ) || !coplanar({}, {}, {1, 0, 0}, {0, 1, 0})) return false;
        const long double denormal = std::numeric_limits<long double>::denorm_min();
        if(!coplanar(
            {}, {denormal, 0, 0}, {0, denormal, 0},
            {denormal, denormal, 0}
        ) || coplanar(
            {}, {denormal, 0, 0}, {0, denormal, 0},
            {0, 0, denormal}
        )) return false;
        for(int exponent: {
            std::numeric_limits<long double>::min_exponent,
            -100, 100, std::numeric_limits<long double>::max_exponent - 3
        }){
            const long double scale = std::ldexp(1.0L, exponent);
            if(!coplanar(
                {}, {scale, 0, 0}, {0, scale, 0}, {scale, scale, 0}
            ) || coplanar(
                {}, {scale, 0, 0}, {0, scale, 0}, {0, 0, scale}
            )) return false;
        }
        for(const Point3 invalid_point: {
            Point3{std::numeric_limits<long double>::infinity(), 0, 0},
            Point3{std::numeric_limits<long double>::quiet_NaN(), 0, 0}
        }){
            bool invalid = false;
            try{
                (void)coplanar({}, {1, 0, 0}, {0, 1, 0}, invalid_point);
            }catch(const std::invalid_argument&){
                invalid = true;
            }
            if(!invalid) return false;
        }
        return true;
    });
}
