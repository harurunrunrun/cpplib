// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/3d/result/polyhedron/polyhedron_diameter.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    const auto result = polyhedron_diameter(mesh);
    assert(result.has_value());
    assert(std::abs(result->distance - 3.0L) < 1e-12L);
    std::cout << std::llround(result->distance * 1e6L) << '\n';
}
