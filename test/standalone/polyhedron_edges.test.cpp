// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>

#include "../../src/algorithm/geometry/3d/index_set/polyhedron/polyhedron_edges.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    const auto result = polyhedron_edges(mesh);
    assert(result.size() == 18);
    std::cout << result.size() << '\n';
}
