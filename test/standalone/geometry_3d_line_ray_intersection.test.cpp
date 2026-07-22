// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/query/line_ray_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Line3 x_axis{{}, {1, 0, 0}};
        const auto same = line_ray_intersection(x_axis, {{2, 0, 0}, {3, 0, 0}});
        const auto point = line_ray_intersection(x_axis, {{0, -1, 0}, {0, 0, 0}});
        const auto behind = line_ray_intersection(x_axis, {{0, -1, 0}, {0, -2, 0}});
        return std::holds_alternative<Ray3>(same) &&
            std::holds_alternative<Point3>(point) &&
            geometry3d_api_close(std::get<Point3>(point), {}) &&
            std::holds_alternative<std::monostate>(behind);
    });
}
