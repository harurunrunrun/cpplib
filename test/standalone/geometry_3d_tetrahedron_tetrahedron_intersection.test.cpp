// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/tetrahedron_tetrahedron_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Tetrahedron3 first{{}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        const Tetrahedron3 disjoint{
            {3, 3, 3}, {4, 3, 3}, {3, 4, 3}, {3, 3, 4}
        };
        const ConvexPolyhedron3 same =
            tetrahedron_tetrahedron_intersection(first, first);
        const ConvexPolyhedron3 none =
            tetrahedron_tetrahedron_intersection(first, disjoint);
        return same.affine_dimension == 3 && same.vertices.size() == 4 &&
            none.affine_dimension == -1 && none.vertices.empty();
    });
}
