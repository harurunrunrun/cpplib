// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_cap_surface_area.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_throws<std::overflow_error>([]{
        const long double maximum = std::numeric_limits<long double>::max();
        (void)sphere_cap_surface_area({{0, 0, 0}, maximum}, maximum);
    }) || !geometry3d_throws<std::invalid_argument>([]{
        (void)sphere_cap_surface_area({{0, 0, 0}, 1},
            std::numeric_limits<long double>::quiet_NaN());
    })) return 1;
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)sphere_cap_surface_area(sphere, 0.0L);
    })) return 1;
    const long double maximum = std::numeric_limits<long double>::max();
    if(sphere_cap_surface_area({{0, 0, 0}, maximum}, 0.0L) != 0.0L){
        return 1;
    }
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            sphere_cap_surface_area({{0, 0, 0}, 3}, 1),
            6 * GEOMETRY3D_PI
        );
    });
}
