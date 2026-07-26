// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/segment_triangle_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Triangle3 triangle{{}, {2, 0, 0}, {0, 2, 0}};
        const auto point = segment_triangle_intersection(
            {{0.5L, 0.5L, -1}, {0.5L, 0.5L, 1}}, triangle
        );
        const auto segment = segment_triangle_intersection(
            {{-1, 0.5L, 0}, {2, 0.5L, 0}}, triangle
        );
        return std::holds_alternative<Point3>(point) &&
            std::holds_alternative<Segment3>(segment);
    });
}
