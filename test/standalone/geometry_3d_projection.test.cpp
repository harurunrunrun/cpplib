// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/projection.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(
            projection(Line3{{0, 0, 0}, {2, 0, 0}}, {1, 3, 4}),
            {1, 0, 0}
        ) && geometry3d_close_point(
            projection(Plane3{{0, 0, 0}, {0, 0, 2}}, {1, 2, 3}),
            {1, 2, 0}
        );
    });
}
