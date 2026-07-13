// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/dot.hpp"
#include "../../src/algorithm/geometry/3d/projection.hpp"
#include "../../src/algorithm/geometry/3d/closest_point.hpp"
#include "../../src/algorithm/geometry/3d/sphere_volume.hpp"
#include "../../src/algorithm/geometry/3d/geometry_3d.hpp"
#include "../../src/algorithm/geometry/3d/cross.hpp"
#include "../../src/algorithm/geometry/3d/contains.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            dot(cross({1, 0, 0}, {0, 1, 0}), {0, 0, 2}),
            2
        ) && contains(Sphere3{{0, 0, 0}, 1}, {0, 0, 0}) == 1;
    });
}
