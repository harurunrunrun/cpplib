// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/segment_circle_intersections.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{}, {0, 0, 1}, 1};
        const auto secant = segment_circle_intersections({{-2, 0, 0}, {2, 0, 0}}, circle);
        const auto short_segment = segment_circle_intersections({{-0.5L, 0, 0}, {0.5L, 0, 0}}, circle);
        const auto point = segment_circle_intersections({{1, 0, 0}, {1, 0, 0}}, circle);
        return secant.size() == 2 && short_segment.empty() &&
            point.size() == 1 && geometry3d_api_close(point[0], {1, 0, 0});
    });
}
