// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/tetrahedron_plane_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Tetrahedron3 tetrahedron{{}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        const auto triangle = tetrahedron_plane_intersection(
            tetrahedron, {{0.5L, 0, 0}, {1, 1, 1}}
        );
        const auto none = tetrahedron_plane_intersection(
            tetrahedron, {{2, 0, 0}, {1, 1, 1}}
        );
        return std::holds_alternative<Polygon3>(triangle) &&
            std::get<Polygon3>(triangle).size() == 3 &&
            std::holds_alternative<std::monostate>(none);
    });
}
