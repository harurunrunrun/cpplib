// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/triangle_circumcenter.hpp"
#include "geometry_3d_center_radius_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Triangle3 triangle{{1, 2, 5}, {5, 2, 5}, {1, 8, 5}};
        if(!geometry3d_api_close(
            triangle_circumcenter(triangle), {3, 5, 5}
        )) return false;
        for(const long double scale: {1e3000L, 1e-3000L}){
            const Point3 center = triangle_circumcenter(
                geometry3d_extreme_right_triangle(scale)
            );
            if(!geometry3d_extreme_relative_close(center.x, scale)
                || !geometry3d_extreme_relative_close(center.y, scale)
                || center.z != 1e3000L) return false;
        }
        return true;
    });
}
