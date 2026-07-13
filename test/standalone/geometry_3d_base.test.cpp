// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/base.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        Point3 point{1, 2, 3};
        point += {2, -1, 1};
        point *= 2;
        const Line3 line{{0, 0, 0}, point};
        return geometry3d_close_point(point, {6, 2, 8}) &&
            geometry3d_close_point(0.5L * point, {3, 1, 4}) &&
            line.b == point;
    });
}
