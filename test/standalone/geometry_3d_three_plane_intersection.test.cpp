// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/point/three_plane_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Plane3 x{{}, {1, 0, 0}}, y{{}, {0, 1, 0}}, z{{}, {0, 0, 1}};
        const auto point = three_plane_intersection(x, y, z);
        const auto line = three_plane_intersection(x, x, y);
        const auto plane = three_plane_intersection(x, x, x);
        const auto none = three_plane_intersection(x, {{1, 0, 0}, {1, 0, 0}}, z);
        return std::holds_alternative<Point3>(point) &&
            std::holds_alternative<Line3>(line) &&
            std::holds_alternative<Plane3>(plane) &&
            std::holds_alternative<std::monostate>(none);
    });
}
