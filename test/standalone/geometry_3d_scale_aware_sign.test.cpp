// competitive-verifier: STANDALONE

#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/predicate/scale_aware_sign.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(scale_aware_sign(5.0e-9L, 100.0L) != 0) return false;
        if(scale_aware_sign(2.0e-8L, 100.0L) != 1) return false;
        if(scale_aware_sign(-2.0e-8L, 100.0L) != -1) return false;
        if(scale_aware_sign(0.0L, 0.0L, 0.0L, 0.0L) != 0) return false;
        try{
            static_cast<void>(scale_aware_sign(0.0L, -1.0L));
            return false;
        }catch(const std::invalid_argument&){}
        try{
            static_cast<void>(scale_aware_sign(
                std::numeric_limits<long double>::quiet_NaN(), 1.0L
            ));
            return false;
        }catch(const std::invalid_argument&){}
        return true;
    });
}
