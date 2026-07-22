// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/orient_plane_normal.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Plane3 result = orient_plane_normal({{1, 2, 3}, {0, 0, -2}}, {0, 0, 1});
        return geometry3d_api_close(result.point, {1, 2, 3})
            && geometry3d_api_close(result.normal, {0, 0, 2});
    });
}
