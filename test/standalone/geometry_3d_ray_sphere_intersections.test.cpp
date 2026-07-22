// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point_collection/ray_sphere_intersections.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)ray_sphere_intersections(Ray3{{-2, 0, 0}, {2, 0, 0}}, sphere);
    })) return 1;
    return geometry3d_test_main([](){
        const auto points = ray_sphere_intersections(
            {{0, 0, 0}, {1, 0, 0}}, {{0, 0, 0}, 2}
        );
        if(points.size() != 1 ||
            !geometry3d_close_point(points[0], {2, 0, 0})) return false;

        const long double translation = 1e3000L;
        const auto behind = ray_sphere_intersections(
            {{0, 0, translation}, {1e500L, 0, translation}},
            {{-1, 0, translation}, 0.5L}
        );
        return behind.empty();
    });
}
