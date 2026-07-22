// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/query/triangle_triangle_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Triangle3 first{{}, {3, 0, 0}, {0, 3, 0}};
        const Triangle3 second{{1, 0, 0}, {3, 1, 0}, {1, 2, 0}};
        const auto polygon = triangle_triangle_intersection(first, second);
        const auto none = triangle_triangle_intersection(
            first, {{0, 0, 1}, {3, 0, 1}, {0, 3, 1}}
        );
        return std::holds_alternative<Polygon3>(polygon) &&
            std::get<Polygon3>(polygon).size() >= 3 &&
            std::holds_alternative<std::monostate>(none);
    });
}
