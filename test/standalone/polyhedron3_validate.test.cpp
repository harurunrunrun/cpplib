// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/void/polyhedron/polyhedron3_validate.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    geometry3d_validate(mesh);
    Polyhedron3 invalid = mesh;
    invalid.faces.front()[0] = invalid.vertices.size();
    bool rejected = false;
    try{ geometry3d_validate(invalid); }catch(const std::out_of_range&){ rejected = true; }
    assert(rejected);
    std::cout << "OK\n";
}
