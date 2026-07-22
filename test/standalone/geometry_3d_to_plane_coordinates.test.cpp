// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/to_plane_coordinates.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const PlaneCoordinateSystem3 system{{1, 2, 3}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        const Point result = to_plane_coordinates(system, {5, 8, 100});
        return geometry3d_api_close(result.x, 4)
            && geometry3d_api_close(result.y, 6);
    });
}
