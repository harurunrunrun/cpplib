// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point_collection/ray_ray_closest_points.hpp"
#include "geometry_3d_linear_closest_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Ray3 first{{}, {1, 0, 0}};
        const Ray3 second{{0, 2, 0}, {0, 3, 0}};
        const auto points = closest_points(first, second);
        if(!geometry3d_api_close(points.first, {})
            || !geometry3d_api_close(points.second, second.origin)){
            return false;
        }
        const long double r = GEOMETRY3D_LINEAR_TEST_LOCAL;
        const long double t = GEOMETRY3D_LINEAR_TEST_TRANSLATION;
        const auto extreme = closest_points(
            geometry3d_linear_test_ray(),
            Ray3{{0, r, t}, {r, r, t}}
        );
        return geometry3d_linear_test_close(extreme.first, {0, 0, t})
            && geometry3d_linear_test_close(extreme.second, {0, r, t});
    });
}
