// competitive-verifier: STANDALONE

#include <cmath>
#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/abs.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        if(!geometry3d_close_value(abs({1, 2, 2}), 3)) return false;
        const long double denormal = std::numeric_limits<long double>::denorm_min();
        if(abs({denormal, 0, 0}) != denormal) return false;
        for(int exponent: {
            std::numeric_limits<long double>::min_exponent,
            -100, 100, std::numeric_limits<long double>::max_exponent - 4
        }){
            const long double scale = std::ldexp(1.0L, exponent);
            const long double length = abs({scale, -2 * scale, 3 * scale});
            if(!std::isfinite(length) ||
                std::abs(length / scale - std::sqrt(14.0L)) > 1.0e-15L){
                return false;
            }
        }
        bool invalid = false;
        try{
            (void)abs({std::numeric_limits<long double>::infinity(), 0, 0});
        }catch(const std::invalid_argument&){
            invalid = true;
        }
        if(!invalid) return false;
        invalid = false;
        try{
            (void)abs({
                std::numeric_limits<long double>::quiet_NaN(), 0, 0
            });
        }catch(const std::invalid_argument&){
            invalid = true;
        }
        if(!invalid) return false;
        bool overflow = false;
        try{
            const long double maximum = std::numeric_limits<long double>::max();
            (void)abs({maximum, maximum, maximum});
        }catch(const std::overflow_error&){
            overflow = true;
        }
        return overflow;
    });
}
