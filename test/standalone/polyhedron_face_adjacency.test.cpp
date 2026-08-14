// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>

#include "../../src/algorithm/geometry/3d/index_set/polyhedron/polyhedron_face_adjacency.hpp"
#include "polyhedron3_test_fixture.hpp"

int main(){
    Polyhedron3 mesh;
    if(!read_polyhedron3_fixture(std::cin, mesh)) mesh = concave_l_prism_fixture();
    const auto result = polyhedron_face_adjacency(mesh);
    std::size_t adjacent_count = 0;
    for(const auto& face: result){
        for(const std::size_t adjacent: face){
            if(adjacent != mesh.faces.size()) ++adjacent_count;
        }
    }
    assert(adjacent_count == mesh.faces.size() * 3);
    std::cout << adjacent_count << '\n';
}
