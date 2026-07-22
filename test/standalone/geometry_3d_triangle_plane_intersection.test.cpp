// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/query/triangle_plane_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Triangle3 triangle{{}, {2, 0, 0}, {0, 2, 0}};
        const auto segment = triangle_plane_intersection(
            triangle, {{0.5L, 0, 0}, {1, 0, 0}}
        );
        const auto all = triangle_plane_intersection(triangle, {{}, {0, 0, 1}});
        const auto none = triangle_plane_intersection(
            triangle, {{0, 0, 1}, {0, 0, 1}}
        );
        return std::holds_alternative<Segment3>(segment) &&
            std::holds_alternative<Polygon3>(all) &&
            std::holds_alternative<std::monostate>(none);
    });
}
