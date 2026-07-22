// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/sphere_plane_cut_volume.hpp"
#include "../../src/algorithm/geometry/3d/scalar/sphere_volume.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)sphere_plane_cut_volume(sphere, Plane3{{0, 0, 0}, {0, 0, 1}}, 1);
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{0, 0, 1}, 3};
        const Plane3 plane{{0, 0, 0}, {0, 0, 1}};
        const long double positive = sphere_plane_cut_volume(sphere, plane, 1);
        const long double negative = sphere_plane_cut_volume(sphere, plane, -1);
        return geometry3d_api_close(positive + negative, sphere_volume(sphere)) &&
            positive > negative &&
            geometry3d_api_close(
                sphere_plane_cut_volume(sphere, {{0, 0, 10}, {0, 0, 1}}, 1),
                0
            );
    });
}
