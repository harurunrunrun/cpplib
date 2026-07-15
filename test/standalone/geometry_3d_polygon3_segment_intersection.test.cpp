// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/polygon3_segment_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{
            {-3, -2, 0}, {3, -2, 0}, {3, 2, 0}, {1, 2, 0},
            {1, 0, 0}, {-1, 0, 0}, {-1, 2, 0}, {-3, 2, 0},
        };
        const auto result = polygon3_segment_intersection(
            polygon, {{-4, 1, 0}, {4, 1, 0}}
        );
        const auto point = polygon3_segment_intersection(
            polygon, {{0, -1, 0}, {0, -1, 0}}
        );
        return result.segments.size() == 2 && point.points.size() == 1;
    });
}
