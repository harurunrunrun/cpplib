// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/convex_polyhedron_segment_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto segment = convex_polyhedron_segment_intersection(
            geometry3d_test_cube(), {{-2, 0, 0}, {2, 0, 0}}
        );
        const auto point = convex_polyhedron_segment_intersection(
            geometry3d_test_cube(), {{0, 0, 0}, {0, 0, 0}}
        );
        return segment && point
            && geometry3d_convex_close(segment->a, {-1, 0, 0})
            && geometry3d_convex_close(segment->b, {1, 0, 0});
    });
}
