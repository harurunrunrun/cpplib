// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/ray_segment_closest_points.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Ray3 ray{{}, {1, 0, 0}};
        const Segment3 segment{{-1, -1, 0}, {-1, 1, 0}};
        const auto points = closest_points(ray, segment);
        const auto reversed = closest_points(segment, ray);
        return geometry3d_api_close(points.first, {}) &&
            geometry3d_api_close(points.second, {-1, 0, 0}) &&
            geometry3d_api_close(reversed.first, {-1, 0, 0});
    });
}
