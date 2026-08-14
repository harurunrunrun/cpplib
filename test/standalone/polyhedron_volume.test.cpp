// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/3d/scalar/polyhedron/polyhedron_volume.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    const long double result = polyhedron_volume(mesh);
    assert(std::abs(result - 3.0L) < 1e-12L);
    std::cout << std::llround(result * 1e6L) << '\n';
}
