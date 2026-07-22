// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/sphere_volume.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_throws<std::overflow_error>([]{
        (void)sphere_volume({{0, 0, 0}, std::numeric_limits<long double>::max()});
    })) return 1;
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)sphere_volume(sphere);
    })) return 1;
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            sphere_volume({{0, 0, 0}, 3}),
            36 * GEOMETRY3D_PI
        );
    });
}
