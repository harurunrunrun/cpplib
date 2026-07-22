// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point_collection/ray_circle_intersections.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_circles([](const Circle3& circle){
        (void)ray_circle_intersections(Ray3{{-2, 0, 0}, {2, 0, 0}}, circle);
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{}, {0, 0, 1}, 1};
        const auto from_center = ray_circle_intersections({{}, {1, 0, 0}}, circle);
        const auto toward = ray_circle_intersections({{-2, 0, 0}, {-1, 0, 0}}, circle);
        const auto away = ray_circle_intersections({{-2, 0, 0}, {-3, 0, 0}}, circle);
        const long double translation = 1e3000L;
        const auto far_behind = ray_circle_intersections(
            {{0, 0, translation}, {1e500L, 0, translation}},
            {{-1, 0, translation}, {0, 0, 1}, 0.5L}
        );
        return from_center.size() == 1 && geometry3d_api_close(from_center[0], {1, 0, 0}) &&
            toward.size() == 2 && away.empty() && far_behind.empty();
    });
}
