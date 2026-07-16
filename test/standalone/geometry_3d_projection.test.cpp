// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/projection.hpp"
#include "geometry_3d_linear_closest_extreme_test_common.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        if(!geometry3d_close_point(
            projection(Line3{{0, 0, 0}, {2, 0, 0}}, {1, 3, 4}),
            {1, 0, 0}
        ) || !geometry3d_close_point(
            projection(Plane3{{0, 0, 0}, {0, 0, 2}}, {1, 2, 3}),
            {1, 2, 0}
        )) return false;
        const long double r = GEOMETRY3D_LINEAR_TEST_LOCAL;
        const long double t = GEOMETRY3D_LINEAR_TEST_TRANSLATION;
        const Point3 query{r / 4, r, t};
        const Point3 expected{r / 4, 0, t};
        return geometry3d_linear_test_close(
            projection(geometry3d_linear_test_line(), query), expected
        ) && geometry3d_linear_test_close(
            projection(Plane3{{0, 0, t}, {0, 1e3000L, 0}}, query),
            expected
        );
    });
}
