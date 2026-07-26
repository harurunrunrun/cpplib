// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_polyhedron_face_adjacency.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto cube = geometry3d_test_cube();
        const auto adjacency = convex_polyhedron_face_adjacency(cube);
        if(adjacency.size() != cube.faces.size()) return false;
        for(const auto& neighbors: adjacency){
            for(std::size_t neighbor: neighbors){
                if(neighbor >= cube.faces.size()) return false;
            }
        }
        return true;
    });
}
