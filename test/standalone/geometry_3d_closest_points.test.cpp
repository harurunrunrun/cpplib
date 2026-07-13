// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/closest_points.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const auto line_points = closest_points(
            Line3{{0, 0, 0}, {1, 0, 0}},
            Line3{{0, 2, 0}, {1, 2, 0}}
        );
        const auto segment_points = closest_points(
            Segment3{{0, 0, 0}, {1, 0, 0}},
            Segment3{{3, 0, 0}, {4, 0, 0}}
        );
        return geometry3d_close_point(line_points.first, {0, 0, 0}) &&
            geometry3d_close_point(line_points.second, {0, 2, 0}) &&
            geometry3d_close_point(segment_points.first, {1, 0, 0}) &&
            geometry3d_close_point(segment_points.second, {3, 0, 0});
    });
}
