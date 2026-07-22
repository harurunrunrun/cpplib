// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/on_plane.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Plane3 plane{{0, 0, 2}, {0, 0, 3}};
        return on_plane(plane, {4, 5, 2}) &&
            !on_plane(plane, {4, 5, 3});
    });
}
