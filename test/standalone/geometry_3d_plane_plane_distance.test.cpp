// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/plane_plane_distance.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return geometry3d_api_close(distance(
            Plane3{{}, {0, 0, 1}}, Plane3{{0, 0, 3}, {0, 0, -1}}
        ), 3) && geometry3d_api_close(distance(
            Plane3{{}, {0, 0, 1}}, Plane3{{}, {1, 0, 0}}
        ), 0);
    });
}
