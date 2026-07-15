// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_intersection_volume.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_throws<std::overflow_error>([]{
        const long double maximum = std::numeric_limits<long double>::max();
        (void)sphere_intersection_volume(
            {{0, 0, 0}, maximum}, {{0, 0, 0}, maximum}
        );
    })) return 1;
    const Sphere3 valid_sphere{{0, 0, 0}, 1};
    if(!geometry3d_rejects_invalid_spheres([&](const Sphere3& sphere){
        (void)sphere_intersection_volume(sphere, valid_sphere);
    }) || !geometry3d_rejects_invalid_spheres([&](const Sphere3& sphere){
        (void)sphere_intersection_volume(valid_sphere, sphere);
    })) return 1;
    const long double translation = 1e3000L;
    if(!geometry3d_close_value(
        sphere_intersection_volume(
            {{0, 0, translation}, 1}, {{0, 0, translation}, 1}
        ),
        4 * GEOMETRY3D_PI / 3
    )) return 1;
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            sphere_intersection_volume(
                {{0, 0, 0}, 1}, {{0, 0, 0}, 1}
            ),
            4 * GEOMETRY3D_PI / 3
        ) && geometry3d_close_value(
            sphere_intersection_volume(
                {{0, 0, 0}, 1}, {{2, 0, 0}, 1}
            ),
            0
        );
    });
}
