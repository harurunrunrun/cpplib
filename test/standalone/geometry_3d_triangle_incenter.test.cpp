// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/triangle_incenter.hpp"
#include "geometry_3d_center_radius_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(!geometry3d_api_close(
            triangle_incenter({{0, 0, 2}, {3, 0, 2}, {0, 4, 2}}),
            {1, 1, 2}
        )) return false;
        for(const long double scale: {1e3000L, 1e-3000L}){
            const long double expected =
                (2.0L - std::sqrt(2.0L)) * scale;
            const Point3 center = triangle_incenter(
                geometry3d_extreme_right_triangle(scale)
            );
            if(!geometry3d_extreme_relative_close(center.x, expected)
                || !geometry3d_extreme_relative_close(
                    center.y, expected
                )
                || center.z != 1e3000L){
                return false;
            }
        }
        try{ (void)triangle_incenter({{}, {1, 0, 0}, {2, 0, 0}}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
