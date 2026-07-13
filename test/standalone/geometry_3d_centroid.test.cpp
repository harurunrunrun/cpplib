// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/centroid.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(
            centroid(Triangle3{{0, 0, 0}, {3, 0, 0}, {0, 3, 0}}),
            {1, 1, 0}
        ) && geometry3d_close_point(
            centroid(Tetrahedron3{
                {0, 0, 0}, {4, 0, 0}, {0, 4, 0}, {0, 0, 4}
            }),
            {1, 1, 1}
        );
    });
}
