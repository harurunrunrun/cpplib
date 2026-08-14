// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>

#include "../../src/algorithm/geometry/3d/type/definition/polyhedron3.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    assert(mesh.vertices.size() == 12);
    assert(mesh.faces.size() == 20);
    std::cout << mesh.vertices.size() << ' ' << mesh.faces.size() << '\n';
}
