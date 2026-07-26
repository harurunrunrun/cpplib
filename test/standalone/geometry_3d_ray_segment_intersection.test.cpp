// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/ray_segment_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Ray3 ray{{}, {1, 0, 0}};
        const auto inside = ray_segment_intersection(ray, {{1, 0, 0}, {3, 0, 0}});
        const auto clipped = ray_segment_intersection(ray, {{-1, 0, 0}, {2, 0, 0}});
        const auto point = ray_segment_intersection(ray, {{0, -1, 0}, {0, 1, 0}});
        const auto none = ray_segment_intersection(ray, {{-3, 0, 0}, {-1, 0, 0}});
        return std::holds_alternative<Segment3>(inside) &&
            std::holds_alternative<Segment3>(clipped) &&
            geometry3d_api_close(std::get<Segment3>(clipped).a, {}) &&
            std::holds_alternative<Point3>(point) &&
            std::holds_alternative<std::monostate>(none);
    });
}
