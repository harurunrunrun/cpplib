// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/on_circle.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_circles([](const Circle3& circle){
        (void)on_circle(circle, Point3{1, 0, 0});
    })) return 1;
    return geometry3d_test_main([](){
        const Circle3 circle{{1, 2, 3}, {0, 0, 2}, 4};
        return on_circle(circle, {5, 2, 3}) &&
            !on_circle(circle, {5, 2, 4});
    });
}
