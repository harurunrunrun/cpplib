// competitive-verifier: STANDALONE

#include <variant>

#include "../../src/algorithm/geometry/3d/query/plane_plane_common_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Plane3 horizontal{{0, 0, 0}, {0, 0, 1}};
        const auto line = plane_plane_common_intersection(
            horizontal, {{0, 0, 0}, {0, 1, 0}}
        );
        const auto same = plane_plane_common_intersection(
            horizontal, {{3, -2, 0}, {0, 0, -2}}
        );
        const auto empty = plane_plane_common_intersection(
            horizontal, {{0, 0, 1}, {0, 0, 1}}
        );
        return std::holds_alternative<Line3>(line)
            && std::holds_alternative<Plane3>(same)
            && std::holds_alternative<std::monostate>(empty);
    });
}
