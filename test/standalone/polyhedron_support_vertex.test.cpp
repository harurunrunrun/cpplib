// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>

#include "../../src/algorithm/geometry/3d/index/polyhedron_point/polyhedron_support_vertex.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    Point3 direction{1, 0, 0};
    std::cin >> direction.x >> direction.y >> direction.z;
    const std::size_t result = polyhedron_support_vertex(mesh, direction);
    assert(result == 1);
    std::cout << result << '\n';
}
