// competitive-verifier: STANDALONE

#include <cstddef>

#include "../../src/algorithm/geometry/3d/shape/plane_arrangement_result3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](auto&, std::size_t){
        PlaneArrangement3 arrangement;
        arrangement.planes.push_back({{0, 0, 0}, {1, 0, 0}});
        arrangement.intersection_lines.push_back({{0, 0, 0}, {0, 0, 1}});
        arrangement.intersection_points.push_back({0, 0, 0});
        arrangement.region_count = 2;
        return arrangement.planes.size() == 1
            && arrangement.intersection_lines.size() == 1
            && arrangement.region_count == 2;
    });
}
