// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/tetrahedron_point_distance.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Tetrahedron3 tetrahedron{{}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        return geometry3d_api_close(distance(tetrahedron, {2, 0, 0}), 1) &&
            geometry3d_api_close(distance(tetrahedron, {0.1L, 0.1L, 0.1L}), 0);
    });
}
