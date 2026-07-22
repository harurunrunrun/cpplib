// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/query/ray_tetrahedron_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Tetrahedron3 tetrahedron{{}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        const auto hit = ray_tetrahedron_intersection(
            {{-1, 0.2L, 0.2L}, {0, 0.2L, 0.2L}}, tetrahedron
        );
        const auto away = ray_tetrahedron_intersection(
            {{-1, 0.2L, 0.2L}, {-2, 0.2L, 0.2L}}, tetrahedron
        );
        return std::holds_alternative<Segment3>(hit) &&
            std::holds_alternative<std::monostate>(away);
    });
}
