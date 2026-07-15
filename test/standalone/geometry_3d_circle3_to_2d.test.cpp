// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/circle3_to_2d.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto system = plane_coordinate_system({{1, 2, 3}, {0, 0, 1}});
        const Circle result = circle3_to_2d({{3, 5, 3}, {0, 0, 2}, 4}, system);
        return geometry3d_api_close(result.center.x, 3)
            && geometry3d_api_close(result.center.y, -2)
            && geometry3d_api_close(result.radius, 4);
    });
}
