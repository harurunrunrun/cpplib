// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/segment_projection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Segment3 segment{{1, 0, 0}, {3, 0, 0}};
        return geometry3d_api_close(projection(segment, {2, 4, 0}), {2, 0, 0}) &&
            geometry3d_api_close(projection(segment, {5, 1, 0}), segment.b) &&
            geometry3d_api_close(projection(Segment3{{1, 2, 3}, {1, 2, 3}}, {}), {1, 2, 3});
    });
}
