// competitive-verifier: STANDALONE

#include <variant>

#include "../../src/algorithm/geometry/3d/segment_plane_common_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Plane3 plane{{0, 0, 0}, {0, 0, 1}};
        const auto point = segment_plane_common_intersection(
            {{1, 2, -1}, {1, 2, 1}}, plane
        );
        const auto contained = segment_plane_common_intersection(
            {{0, 0, 0}, {1, 0, 0}}, plane
        );
        const auto empty = segment_plane_common_intersection(
            {{0, 0, 1}, {1, 0, 1}}, plane
        );
        const auto degenerate = segment_plane_common_intersection(
            {{2, 3, 0}, {2, 3, 0}}, plane
        );
        return std::holds_alternative<Point3>(point)
            && geometry3d_api_close(std::get<Point3>(point), {1, 2, 0})
            && std::holds_alternative<Segment3>(contained)
            && std::holds_alternative<std::monostate>(empty)
            && std::holds_alternative<Point3>(degenerate);
    });
}
