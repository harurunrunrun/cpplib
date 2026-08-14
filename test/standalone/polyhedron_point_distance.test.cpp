// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/3d/scalar/polyhedron_point/polyhedron_point_distance.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    Point3 point{1.5L, 1.75L, 0.5L};
    std::cin >> point.x >> point.y >> point.z;
    const long double result = polyhedron_point_distance(mesh, point);
    assert(std::abs(result - 0.5L) < 1e-12L);
    std::cout << std::llround(result * 1e6L) << '\n';
}
