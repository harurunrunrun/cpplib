// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/segment_plane/segment_plane_distance_segment_plane.hpp"
#include "../../src/algorithm/geometry/3d/scalar/plane_segment/segment_plane_distance_plane_segment.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/segment_plane_distance.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return geometry3d_api_close(distance(
            Segment3{{0, 0, 2}, {1, 0, 3}}, Plane3{{}, {0, 0, 1}}
        ), 2) && geometry3d_api_close(distance(
            Plane3{{}, {0, 0, 1}}, Segment3{{0, 0, -1}, {0, 0, 1}}
        ), 0);
    });
}
