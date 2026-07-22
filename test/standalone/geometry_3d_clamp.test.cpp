// competitive-verifier: STANDALONE

#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/scalar/clamp.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(::clamp(-2.0L, -1.0L, 3.0L) != -1.0L) return false;
        if(::clamp(2.0L, -1.0L, 3.0L) != 2.0L) return false;
        if(::clamp(5.0L, -1.0L, 3.0L) != 3.0L) return false;
        try{
            static_cast<void>(::clamp(0.0L, 1.0L, -1.0L));
            return false;
        }catch(const std::invalid_argument&){}
        try{
            static_cast<void>(::clamp(
                std::numeric_limits<long double>::infinity(), -1.0L, 1.0L
            ));
            return false;
        }catch(const std::invalid_argument&){}
        return true;
    });
}
