// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/ray_segment_closest_points.hpp"
#include "geometry_3d_linear_closest_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Ray3 ray{{}, {1, 0, 0}};
        const Segment3 segment{{-1, -1, 0}, {-1, 1, 0}};
        const auto points = closest_points(ray, segment);
        const auto reversed = closest_points(segment, ray);
        if(!geometry3d_api_close(points.first, {}) ||
            !geometry3d_api_close(points.second, {-1, 0, 0}) ||
            !geometry3d_api_close(reversed.first, {-1, 0, 0})){
            return false;
        }
        const long double r = GEOMETRY3D_LINEAR_TEST_LOCAL;
        const long double t = GEOMETRY3D_LINEAR_TEST_TRANSLATION;
        const Segment3 parallel{{0, r, t}, {r, r, t}};
        const auto extreme = closest_points(
            geometry3d_linear_test_ray(), parallel
        );
        const auto extreme_reversed = closest_points(
            parallel, geometry3d_linear_test_ray()
        );
        return geometry3d_linear_test_close(extreme.first, {0, 0, t})
            && geometry3d_linear_test_close(extreme.second, {0, r, t})
            && geometry3d_linear_test_close(extreme_reversed.first, {0, r, t})
            && geometry3d_linear_test_close(extreme_reversed.second, {0, 0, t});
    });
}
