// competitive-verifier: STANDALONE

#include <cmath>
#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/unit.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        if(!geometry3d_close_point(unit({0, 3, 4}), {0, 0.6L, 0.8L})){
            return false;
        }
        const long double maximum = std::numeric_limits<long double>::max();
        const Point3 huge = unit({maximum, maximum, 0});
        if(!geometry3d_close_point(
            huge, {std::sqrt(0.5L), std::sqrt(0.5L), 0}, 1.0e-15L
        )) return false;
        const long double denormal = std::numeric_limits<long double>::denorm_min();
        if(!geometry3d_close_point(unit({denormal, 0, 0}), {1, 0, 0})){
            return false;
        }
        for(int exponent: {
            std::numeric_limits<long double>::min_exponent,
            -100, 100, std::numeric_limits<long double>::max_exponent - 2
        }){
            const long double scale = std::ldexp(1.0L, exponent);
            const Point3 direction = unit({scale, -scale, 0});
            if(!geometry3d_close_point(
                direction,
                {std::sqrt(0.5L), -std::sqrt(0.5L), 0},
                1.0e-15L
            )) return false;
        }
        for(const Point3 invalid_point: {
            Point3{},
            Point3{std::numeric_limits<long double>::infinity(), 0, 0},
            Point3{std::numeric_limits<long double>::quiet_NaN(), 0, 0}
        }){
            bool invalid = false;
            try{
                (void)unit(invalid_point);
            }catch(const std::invalid_argument&){
                invalid = true;
            }
            if(!invalid) return false;
        }
        return true;
    });
}
