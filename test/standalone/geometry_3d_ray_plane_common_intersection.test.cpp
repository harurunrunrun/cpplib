// competitive-verifier: STANDALONE

#include <variant>

#include "../../src/algorithm/geometry/3d/query/ray_plane_common_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Plane3 plane{{0, 0, 0}, {0, 0, 1}};
        const auto point = ray_plane_common_intersection(
            {{1, 2, 1}, {1, 2, -1}}, plane
        );
        const auto behind = ray_plane_common_intersection(
            {{1, 2, 1}, {1, 2, 2}}, plane
        );
        const auto contained = ray_plane_common_intersection(
            {{0, 0, 0}, {1, 0, 0}}, plane
        );
        const auto parallel_empty = ray_plane_common_intersection(
            {{0, 0, 1}, {1, 0, 1}}, plane
        );
        return std::holds_alternative<Point3>(point)
            && geometry3d_api_close(std::get<Point3>(point), {1, 2, 0})
            && std::holds_alternative<std::monostate>(behind)
            && std::holds_alternative<Ray3>(contained)
            && std::holds_alternative<std::monostate>(parallel_empty);
    });
}
