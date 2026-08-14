// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

#include "../../src/algorithm/geometry/3d/result/polyhedron_segment/polyhedron_segment_intersection.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    Segment3 segment{{-1, 0.5L, 0.5L}, {3, 0.5L, 0.5L}};
    std::cin >> segment.a.x >> segment.a.y >> segment.a.z
        >> segment.b.x >> segment.b.y >> segment.b.z;
    const auto result = polyhedron_segment_intersection(mesh, segment);
    assert(result.size() == 1);
    assert(std::abs(result[0].a.x) < 1e-12L);
    assert(std::abs(result[0].b.x - 2.0L) < 1e-12L);
    Polyhedron3 disconnected = concave_l_prism_fixture();
    Polyhedron3 shifted = concave_l_prism_fixture();
    const std::size_t offset = disconnected.vertices.size();
    for(Point3& vertex: shifted.vertices){
        vertex.x += 4.0L;
        disconnected.vertices.push_back(vertex);
    }
    for(auto face: shifted.faces){
        face[0] += offset;
        face[1] += offset;
        face[2] += offset;
        disconnected.faces.push_back(face);
    }
    const auto multiple = polyhedron_segment_intersection(
        disconnected, {{-1, 0.5L, 0.5L}, {7, 0.5L, 0.5L}}
    );
    assert(multiple.size() == 2);
    std::cout << result.size() << ' '
        << std::llround(result[0].a.x * 1e6L) << ' '
        << std::llround(result[0].a.y * 1e6L) << ' '
        << std::llround(result[0].a.z * 1e6L) << ' '
        << std::llround(result[0].b.x * 1e6L) << ' '
        << std::llround(result[0].b.y * 1e6L) << ' '
        << std::llround(result[0].b.z * 1e6L) << '\n';
}
