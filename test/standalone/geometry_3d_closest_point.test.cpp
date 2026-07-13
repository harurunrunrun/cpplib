// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/closest_point.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(
            closest_point(Ray3{{0, 0, 0}, {1, 0, 0}}, {-2, 3, 0}),
            {0, 0, 0}
        ) && geometry3d_close_point(
            closest_point(Segment3{{0, 0, 0}, {2, 0, 0}}, {3, 4, 0}),
            {2, 0, 0}
        ) && geometry3d_close_point(
            closest_point(
                Triangle3{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}},
                {2, 2, 0}
            ),
            {1, 1, 0}
        );
    });
}
