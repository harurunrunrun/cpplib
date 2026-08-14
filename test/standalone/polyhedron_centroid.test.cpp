// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/3d/point/polyhedron/polyhedron_centroid.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    const Point3 result = polyhedron_centroid(mesh);
    assert(std::abs(result.x - 5.0L / 6.0L) < 1e-12L);
    assert(std::abs(result.y - 5.0L / 6.0L) < 1e-12L);
    assert(std::abs(result.z - 0.5L) < 1e-12L);
    std::cout << std::llround(result.x * 1e6L) << ' '
        << std::llround(result.y * 1e6L) << ' '
        << std::llround(result.z * 1e6L) << '\n';
}
