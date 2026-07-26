// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/from_plane_coordinates.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const PlaneCoordinateSystem3 system{{1, 2, 3}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        return geometry3d_api_close(from_plane_coordinates(system, {4, 6}), {5, 8, 3});
    });
}
