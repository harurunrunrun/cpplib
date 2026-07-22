// competitive-verifier: STANDALONE

#include <stdexcept>

#include "../../src/algorithm/geometry/3d/predicate/insphere.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Point3 a{1, 1, 1}, b{-1, -1, 1};
        const Point3 c{-1, 1, -1}, d{1, -1, -1};
        if(!(insphere(a, b, c, d, {0, 0, 0}) > 0.0L)) return false;
        if(insphere(a, b, c, d, a) != 0.0L) return false;
        if(!(insphere(a, b, c, d, {4, 0, 0}) < 0.0L)) return false;
        if(!(insphere(a, c, b, d, {0, 0, 0}) > 0.0L)) return false;
        try{
            static_cast<void>(insphere(
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1}
            ));
            return false;
        }catch(const std::invalid_argument&){}
        return true;
    });
}
