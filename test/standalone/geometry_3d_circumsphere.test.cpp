// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/circumsphere.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Sphere3 sphere = circumsphere({
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
        });
        return geometry3d_close_point(sphere.center, {0.5L, 0.5L, 0.5L}) &&
            geometry3d_close_value(sphere.radius, std::sqrt(3.0L) / 2);
    });
}
