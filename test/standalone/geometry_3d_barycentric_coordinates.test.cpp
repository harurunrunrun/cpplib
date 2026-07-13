// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/barycentric_coordinates.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const auto triangle = barycentric_coordinates(
            Triangle3{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}},
            {0.5L, 0.5L, 0}
        );
        const auto tetrahedron = barycentric_coordinates(
            Tetrahedron3{
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
            },
            {0.1L, 0.2L, 0.3L}
        );
        return geometry3d_close_value(triangle[0], 0.5L) &&
            geometry3d_close_value(triangle[1], 0.25L) &&
            geometry3d_close_value(triangle[2], 0.25L) &&
            geometry3d_close_value(tetrahedron[0], 0.4L) &&
            geometry3d_close_value(tetrahedron[1], 0.1L) &&
            geometry3d_close_value(tetrahedron[2], 0.2L) &&
            geometry3d_close_value(tetrahedron[3], 0.3L);
    });
}
