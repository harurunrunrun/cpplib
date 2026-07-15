// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/segment_segment_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto overlap_value = segment_segment_intersection(
            {{0, 0, 0}, {3, 0, 0}}, {{1, 0, 0}, {4, 0, 0}}
        );
        const auto touch = segment_segment_intersection(
            {{0, 0, 0}, {1, 0, 0}}, {{1, 0, 0}, {2, 0, 0}}
        );
        const auto cross_value = segment_segment_intersection(
            {{-1, 0, 0}, {1, 0, 0}}, {{0, -1, 0}, {0, 1, 0}}
        );
        const auto skew_value = segment_segment_intersection(
            {{-1, 0, 0}, {1, 0, 0}}, {{0, -1, 1}, {0, 1, 1}}
        );
        return std::holds_alternative<Segment3>(overlap_value) &&
            std::holds_alternative<Point3>(touch) &&
            std::holds_alternative<Point3>(cross_value) &&
            std::holds_alternative<std::monostate>(skew_value);
    });
}
