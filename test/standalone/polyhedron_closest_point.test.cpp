// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/3d/point/polyhedron_point/polyhedron_closest_point.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    Point3 point{1.5L, 1.75L, 0.5L};
    std::cin >> point.x >> point.y >> point.z;
    const Point3 result = polyhedron_closest_point(mesh, point);
    assert(std::abs(result.x - 1.0L) < 1e-12L);
    assert(std::abs(result.y - 1.75L) < 1e-12L);
    assert(std::abs(result.z - 0.5L) < 1e-12L);
    std::cout << std::llround(result.x * 1e6L) << ' '
        << std::llround(result.y * 1e6L) << ' '
        << std::llround(result.z * 1e6L) << '\n';
}
