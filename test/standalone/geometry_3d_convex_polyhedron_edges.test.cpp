// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/query/convex_polyhedron_edges.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto cube = geometry3d_test_cube();
        if(convex_polyhedron_edges(cube).size() != 12) return false;
        ConvexPolyhedron3 invalid = cube;
        invalid.faces.front()[0] = invalid.vertices.size();
        try{ (void)convex_polyhedron_edges(invalid); }
        catch(const std::out_of_range&){ return true; }
        return false;
    });
}
