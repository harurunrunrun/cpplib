// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/point_point/dot.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/projection.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/closest_point.hpp"
#include "../../src/algorithm/geometry/3d/scalar/sphere/sphere_volume.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/geometry_3d.hpp"
#include "../../src/algorithm/geometry/3d/point/point_point/cross.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/contains.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            dot(cross({1, 0, 0}, {0, 1, 0}), {0, 0, 2}),
            2
        ) && contains(Sphere3{{0, 0, 0}, 1}, {0, 0, 0}) == 1;
    });
}
