// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/ray_ray_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Ray3 forward{{}, {1, 0, 0}};
        const auto ray = ray_ray_intersection(forward, {{2, 0, 0}, {3, 0, 0}});
        const auto segment = ray_ray_intersection(forward, {{2, 0, 0}, {1, 0, 0}});
        const auto point = ray_ray_intersection(forward, {{}, {-1, 0, 0}});
        const auto none = ray_ray_intersection(forward, {{-1, 0, 0}, {-2, 0, 0}});
        const auto cross_value = ray_ray_intersection(forward, {{0, -1, 0}, {0, 1, 0}});
        return std::holds_alternative<Ray3>(ray) &&
            std::holds_alternative<Segment3>(segment) &&
            std::holds_alternative<Point3>(point) &&
            std::holds_alternative<std::monostate>(none) &&
            std::holds_alternative<Point3>(cross_value);
    });
}
