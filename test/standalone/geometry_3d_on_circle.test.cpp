// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/on_circle.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Circle3 circle{{1, 2, 3}, {0, 0, 2}, 4};
        return on_circle(circle, {5, 2, 3}) &&
            !on_circle(circle, {5, 2, 4});
    });
}
