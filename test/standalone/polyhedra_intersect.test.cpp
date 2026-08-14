// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>

#include "../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/polyhedra_intersect.hpp"
#include "polyhedron3_test_fixture.hpp"

namespace{

Polyhedron3 tetrahedron(long double offset){
    return {
        {{offset + 0.2L, offset + 0.2L, 0.2L},
         {offset + 0.8L, offset + 0.2L, 0.2L},
         {offset + 0.2L, offset + 0.8L, 0.2L},
         {offset + 0.2L, offset + 0.2L, 0.8L}},
        {{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}},
    };
}

}  // namespace

int main(){
    Polyhedron3 first;
    Polyhedron3 second;
    if(!read_polyhedron3_fixture(std::cin, first)){
        first = concave_l_prism_fixture();
        second = tetrahedron(0.0L);
    }else{
        const bool read_second = read_polyhedron3_fixture(std::cin, second);
        assert(read_second);
    }
    assert(polyhedra_intersect(concave_l_prism_fixture(), tetrahedron(0.0L)));
    assert(!polyhedra_intersect(concave_l_prism_fixture(), tetrahedron(1.0L)));
    std::cout << polyhedra_intersect(first, second) << '\n';
}
