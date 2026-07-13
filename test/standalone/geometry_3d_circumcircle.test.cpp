// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/circumcircle.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Circle3 circle = circumcircle(
            {{0, 0, 0}, {2, 0, 0}, {0, 2, 0}}
        );
        return geometry3d_close_point(circle.center, {1, 1, 0}) &&
            geometry3d_close_point(circle.normal, {0, 0, 1}) &&
            geometry3d_close_value(circle.radius, std::sqrt(2.0L));
    });
}
