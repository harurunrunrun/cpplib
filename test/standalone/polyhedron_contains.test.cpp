// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>

#include "../../src/algorithm/geometry/3d/predicate/polyhedron_point/polyhedron_contains.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    Point3 point{0.5L, 1.5L, 0.5L};
    std::cin >> point.x >> point.y >> point.z;
    assert(polyhedron_contains(mesh, {0.5L, 1.5L, 0.5L}));
    assert(!polyhedron_contains(mesh, {1.5L, 1.5L, 0.5L}));
    std::cout << polyhedron_contains(mesh, point) << '\n';
}
