// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/triangle_circumradius.hpp"
#include "geometry_3d_center_radius_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(!geometry3d_api_close(
            triangle_circumradius({{}, {3, 0, 0}, {0, 4, 0}}), 2.5L
        )) return false;
        for(const long double scale: {1e3000L, 1e-3000L}){
            if(!geometry3d_extreme_relative_close(
                triangle_circumradius(
                    geometry3d_extreme_right_triangle(scale)
                ),
                std::sqrt(2.0L) * scale
            )) return false;
        }
        return true;
    });
}
