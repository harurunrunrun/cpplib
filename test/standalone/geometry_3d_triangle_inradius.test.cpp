// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/triangle_inradius.hpp"
#include "geometry_3d_center_radius_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(!geometry3d_api_close(
            triangle_inradius({{}, {3, 0, 0}, {0, 4, 0}}), 1
        )) return false;
        for(const long double scale: {1e3000L, 1e-3000L}){
            const long double expected =
                (2.0L - std::sqrt(2.0L)) * scale;
            if(!geometry3d_extreme_relative_close(
                triangle_inradius(
                    geometry3d_extreme_right_triangle(scale)
                ),
                expected
            )) return false;
        }
        try{ (void)triangle_inradius({{}, {1, 1, 1}, {2, 2, 2}}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
