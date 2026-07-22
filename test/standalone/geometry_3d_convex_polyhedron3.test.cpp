// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/core/convex_polyhedron3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const ConvexPolyhedron3 polyhedron{
            2,
            {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}},
            {{0, 1, 2}},
        };
        return polyhedron.affine_dimension == 2
            && polyhedron.vertices.size() == 3
            && polyhedron.faces.size() == 1;
    });
}
