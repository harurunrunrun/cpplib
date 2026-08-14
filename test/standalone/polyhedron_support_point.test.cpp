// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/3d/point/polyhedron_point/polyhedron_support_point.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    Point3 direction{1, 0, 0};
    std::cin >> direction.x >> direction.y >> direction.z;
    const Point3 result = polyhedron_support_point(mesh, direction);
    assert(result == Point3(2, 0, 0));
    std::cout << std::llround(result.x * 1e6L) << ' '
        << std::llround(result.y * 1e6L) << ' '
        << std::llround(result.z * 1e6L) << '\n';
}
