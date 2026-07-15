// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/segment_circle_intersections.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_circles([](const Circle3& circle){
        (void)segment_circle_intersections(Segment3{{-2, 0, 0}, {2, 0, 0}}, circle);
    })) return 1;
    const long double infinity = std::numeric_limits<long double>::infinity();
    if(!geometry3d_throws<std::invalid_argument>([&]{
        (void)segment_circle_intersections(
            Segment3{{infinity, 0, 0}, {infinity, 0, 0}},
            Circle3{{0, 0, 0}, {0, 0, 1}, 1}
        );
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{}, {0, 0, 1}, 1};
        const auto secant = segment_circle_intersections({{-2, 0, 0}, {2, 0, 0}}, circle);
        const auto short_segment = segment_circle_intersections({{-0.5L, 0, 0}, {0.5L, 0, 0}}, circle);
        const auto point = segment_circle_intersections({{1, 0, 0}, {1, 0, 0}}, circle);
        const long double maximum =
            std::numeric_limits<long double>::max();
        const auto one_representable_root = segment_circle_intersections(
            {{0, 0, 0}, {1, 0, 0}},
            {{maximum * 0.75L, 0, 0}, {0, 0, 1}, maximum * 0.75L}
        );
        return secant.size() == 2 && short_segment.empty() &&
            point.size() == 1 && geometry3d_api_close(point[0], {1, 0, 0}) &&
            one_representable_root.size() == 1 &&
            one_representable_root.front().x == 0.0L;
    });
}
