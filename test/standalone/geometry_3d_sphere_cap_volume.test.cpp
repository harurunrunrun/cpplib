// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/sphere_cap_volume.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_throws<std::overflow_error>([]{
        const long double maximum = std::numeric_limits<long double>::max();
        (void)sphere_cap_volume({{0, 0, 0}, maximum}, maximum);
    }) || !geometry3d_throws<std::invalid_argument>([]{
        (void)sphere_cap_volume({{0, 0, 0}, 1},
            std::numeric_limits<long double>::infinity());
    })) return 1;
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)sphere_cap_volume(sphere, 0.0L);
    })) return 1;
    const long double tiny = 1e-3000L;
    const long double mixed = sphere_cap_volume(
        {{0, 0, 0}, 1e3000L}, tiny
    );
    if(std::abs(mixed / tiny - GEOMETRY3D_PI) > 1e-12L) return 1;
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            sphere_cap_volume({{0, 0, 0}, 3}, 1),
            8 * GEOMETRY3D_PI / 3
        );
    });
}
