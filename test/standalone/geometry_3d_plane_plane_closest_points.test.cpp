// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/plane_plane_closest_points.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Plane3 first{{}, {0, 0, 1}};
        const Plane3 second{{0, 0, 3}, {0, 0, -2}};
        const auto points = closest_points(first, second);
        return geometry3d_api_close(points.first, {}) &&
            geometry3d_api_close(points.second, {0, 0, 3});
    });
}
