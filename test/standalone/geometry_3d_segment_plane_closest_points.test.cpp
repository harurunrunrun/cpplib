// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point_collection/segment_plane_closest_points.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Segment3 segment{{0, 0, 2}, {1, 0, 3}};
        const Plane3 plane{{}, {0, 0, 1}};
        const auto points = closest_points(segment, plane);
        return geometry3d_api_close(points.first, segment.a) &&
            geometry3d_api_close(points.second, {});
    });
}
