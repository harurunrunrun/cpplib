// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/plane_coordinate_system.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto system = plane_coordinate_system({{1, 2, 3}, {0, 0, 4}});
        return geometry3d_api_close(system.origin, {1, 2, 3})
            && geometry3d_api_close(abs(system.first_axis), 1)
            && geometry3d_api_close(abs(system.second_axis), 1)
            && geometry3d_api_close(dot(system.first_axis, system.second_axis), 0)
            && geometry3d_api_close(cross(system.first_axis, system.second_axis), system.normal);
    });
}
