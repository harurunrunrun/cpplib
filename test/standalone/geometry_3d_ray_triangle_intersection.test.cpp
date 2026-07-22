// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/query/ray_triangle_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Triangle3 triangle{{}, {2, 0, 0}, {0, 2, 0}};
        const auto hit = ray_triangle_intersection(
            {{0.5L, 0.5L, 1}, {0.5L, 0.5L, 0}}, triangle
        );
        const auto away = ray_triangle_intersection(
            {{0.5L, 0.5L, 1}, {0.5L, 0.5L, 2}}, triangle
        );
        return std::holds_alternative<Point3>(hit) &&
            std::holds_alternative<std::monostate>(away);
    });
}
