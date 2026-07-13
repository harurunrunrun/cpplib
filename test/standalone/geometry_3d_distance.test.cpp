// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/distance.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            distance(Point3{0, 0, 0}, Point3{1, 2, 2}), 3
        ) && geometry3d_close_value(
            distance(Line3{{0, 0, 0}, {1, 0, 0}}, {0, 3, 4}), 5
        ) && geometry3d_close_value(
            distance(Ray3{{0, 0, 0}, {1, 0, 0}}, {-2, 0, 0}), 2
        ) && geometry3d_close_value(
            distance(Segment3{{0, 0, 0}, {2, 0, 0}}, {3, 0, 0}), 1
        ) && geometry3d_close_value(
            distance(Plane3{{0, 0, 0}, {0, 0, 1}}, {0, 0, -3}), 3
        ) && geometry3d_close_value(
            distance(
                Line3{{0, 0, 0}, {1, 0, 0}},
                Line3{{0, 2, 0}, {1, 2, 0}}
            ),
            2
        ) && geometry3d_close_value(
            distance(
                Segment3{{0, 0, 0}, {1, 0, 0}},
                Segment3{{3, 0, 0}, {4, 0, 0}}
            ),
            2
        ) && geometry3d_close_value(
            distance(
                Triangle3{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}},
                {0.5L, 0.5L, 3}
            ),
            3
        );
    });
}
