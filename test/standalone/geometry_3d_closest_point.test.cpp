// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/closest_point.hpp"
#include "geometry_3d_linear_closest_extreme_test_common.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        if(!geometry3d_close_point(
            closest_point(Ray3{{0, 0, 0}, {1, 0, 0}}, {-2, 3, 0}),
            {0, 0, 0}
        ) || !geometry3d_close_point(
            closest_point(Segment3{{0, 0, 0}, {2, 0, 0}}, {3, 4, 0}),
            {2, 0, 0}
        ) || !geometry3d_close_point(
            closest_point(
                Triangle3{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}},
                {2, 2, 0}
            ),
            {1, 1, 0}
        )) return false;
        const long double r = GEOMETRY3D_LINEAR_TEST_LOCAL;
        const long double t = GEOMETRY3D_LINEAR_TEST_TRANSLATION;
        if(!geometry3d_linear_test_close(
            closest_point(
                geometry3d_linear_test_ray(), {-r, r, t}
            ),
            {0, 0, t}
        ) || !geometry3d_linear_test_close(
            closest_point(
                geometry3d_linear_test_segment(), {r / 2, r, t}
            ),
            {r / 2, 0, t}
        )){
            return false;
        }
        return geometry3d_linear_test_close(
            closest_point(
                geometry3d_linear_test_triangle(), {2 * r, 2 * r, t}
            ),
            {r / 2, r / 2, t}
        );
    });
}
