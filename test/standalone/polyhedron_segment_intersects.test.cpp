// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>

#include "../../src/algorithm/geometry/3d/predicate/polyhedron_segment/polyhedron_segment_intersects.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    Segment3 segment{{0.5L, 0.5L, -1.0L}, {0.5L, 0.5L, 2.0L}};
    std::cin >> segment.a.x >> segment.a.y >> segment.a.z
        >> segment.b.x >> segment.b.y >> segment.b.z;
    assert(polyhedron_segment_intersects(
        mesh, {{0.5L, 0.5L, -1.0L}, {0.5L, 0.5L, 2.0L}}
    ));
    assert(!polyhedron_segment_intersects(
        mesh, {{1.5L, 1.5L, -1.0L}, {1.5L, 1.5L, 2.0L}}
    ));
    std::cout << polyhedron_segment_intersects(mesh, segment) << '\n';
}
