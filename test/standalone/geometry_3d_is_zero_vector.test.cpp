// competitive-verifier: STANDALONE

#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/is_zero_vector.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(!is_zero_vector({0, 0, 0})) return false;
        if(!is_zero_vector({5.0e-11L, 0, 0})) return false;
        if(is_zero_vector({5.0e-8L, 0, 0})) return false;
        if(!is_zero_vector({5.0e-9L, 0, 0}, 100.0L)) return false;
        try{
            static_cast<void>(is_zero_vector({
                std::numeric_limits<long double>::infinity(), 0, 0
            }));
            return false;
        }catch(const std::invalid_argument&){}
        return true;
    });
}
