// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/query/convex_polyhedron_counts.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto counts = convex_polyhedron_counts(geometry3d_test_cube());
        return counts.vertices == 8 && counts.edges == 12
            && counts.facet_count == 6
            && counts.triangulated_face_count == 12
            && counts.triangular_faces == counts.triangulated_face_count;
    });
}
