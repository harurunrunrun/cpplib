// competitive-verifier: STANDALONE

#include <cmath>

#include "../../src/algorithm/geometry/3d/predicate/segment_point/on_segment.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Segment3 rounded_endpoint_segment{
            {380, 639, -438},
            {529, -641, 284}
        };
        const Point3 rounded_endpoint{
            std::nextafter(380.0L, 381.0L),
            639,
            -438
        };
        return on_segment({{0, 0, 0}, {2, 0, 0}}, {1, 0, 0}) &&
            !on_segment({{0, 0, 0}, {2, 0, 0}}, {3, 0, 0}) &&
            on_segment({{1, 2, 3}, {1, 2, 3}}, {1, 2, 3}) &&
            on_segment(rounded_endpoint_segment, rounded_endpoint);
    });
}
