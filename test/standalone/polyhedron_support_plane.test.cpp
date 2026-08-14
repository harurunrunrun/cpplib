// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/3d/plane/polyhedron_point/polyhedron_support_plane.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    Point3 normal{1, 0, 0};
    std::cin >> normal.x >> normal.y >> normal.z;
    const Plane3 result = polyhedron_support_plane(mesh, normal);
    assert(result.point == Point3(2, 0, 0));
    assert(result.normal == normal);
    std::cout << std::llround(result.point.x * 1e6L) << ' '
        << std::llround(result.point.y * 1e6L) << ' '
        << std::llround(result.point.z * 1e6L) << '\n';
}
