// competitive-verifier: STANDALONE

#include <type_traits>
#include <variant>
#include "../../src/algorithm/geometry/3d/linear_intersection3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        static_assert(std::variant_size_v<LinearIntersection3> == 5);
        LinearIntersection3 value = Point3{1, 2, 3};
        return std::holds_alternative<Point3>(value) &&
            geometry3d_api_close(std::get<Point3>(value), {1, 2, 3});
    });
}
