// competitive-verifier: STANDALONE

#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/predicate/relative_equal.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(!relative_equal(1.0L, 1.0L + 5.0e-11L)) return false;
        if(relative_equal(1.0L, 1.0L + 5.0e-8L)) return false;
        if(!relative_equal(1.0e12L, 1.0e12L + 50.0L)) return false;
        if(relative_equal(1.0e12L, 1.0e12L + 200.0L)) return false;
        try{
            static_cast<void>(relative_equal(
                0.0L, std::numeric_limits<long double>::infinity()
            ));
            return false;
        }catch(const std::invalid_argument&){}
        try{
            static_cast<void>(relative_equal(0.0L, 0.0L, -1.0L));
            return false;
        }catch(const std::invalid_argument&){}
        return true;
    });
}
