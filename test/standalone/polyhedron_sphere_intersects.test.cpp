// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>

#include "../../src/algorithm/geometry/3d/predicate/polyhedron_sphere/polyhedron_sphere_intersects.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    Sphere3 sphere{{1.5L, 1.75L, 0.5L}, 0.5L};
    std::cin >> sphere.center.x >> sphere.center.y >> sphere.center.z >> sphere.radius;
    assert(polyhedron_sphere_intersects(mesh, {{1.5L, 1.75L, 0.5L}, 0.5L}));
    assert(!polyhedron_sphere_intersects(mesh, {{1.5L, 1.75L, 0.5L}, 0.49L}));
    std::cout << polyhedron_sphere_intersects(mesh, sphere) << '\n';
}
