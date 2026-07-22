// competitive-verifier: STANDALONE

#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/predicate/orient3d.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Point3 a{0, 0, 0}, b{1, 0, 0}, c{0, 1, 0}, d{0, 0, 1};
        if(!geometry3d_api_close(orient3d(a, b, c, d), 1.0L)) return false;
        if(!geometry3d_api_close(orient3d(a, c, b, d), -1.0L)) return false;
        if(orient3d(a, b, c, {1, 1, 0}) != 0.0L) return false;
        const Point3 offset{1.0e12L, -2.0e12L, 3.0e12L};
        if(!geometry3d_api_close(
            orient3d(a + offset, b + offset, c + offset, d + offset), 1.0L
        )) return false;
        try{
            static_cast<void>(orient3d(
                a, b, c, {0, 0, std::numeric_limits<long double>::infinity()}
            ));
            return false;
        }catch(const std::invalid_argument&){}
        return true;
    });
}
