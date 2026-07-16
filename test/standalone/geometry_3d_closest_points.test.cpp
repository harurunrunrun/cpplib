// competitive-verifier: STANDALONE

#include <limits>

#include "../../src/algorithm/geometry/3d/closest_points.hpp"
#include "geometry_3d_linear_closest_extreme_test_common.hpp"
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
        if(!geometry3d_close_point(line_points.first, {0, 0, 0}) ||
            !geometry3d_close_point(line_points.second, {0, 2, 0}) ||
            !geometry3d_close_point(segment_points.first, {1, 0, 0}) ||
            !geometry3d_close_point(segment_points.second, {3, 0, 0})){
            return false;
        }
        const auto local_lines = closest_points(
            geometry3d_linear_test_line(),
            geometry3d_linear_test_parallel_line()
        );
        const long double r = GEOMETRY3D_LINEAR_TEST_LOCAL;
        const long double t = GEOMETRY3D_LINEAR_TEST_TRANSLATION;
        if(!geometry3d_linear_test_close(
                local_lines.first, {0, 0, t})
            || !geometry3d_linear_test_close(
                local_lines.second, {0, r, t})){
            return false;
        }
        const long double maximum =
            0.75L * std::numeric_limits<long double>::max();
        const auto extreme_lines = closest_points(
            Line3{{-maximum, 0, 0}, {maximum, 0, 0}},
            Line3{{0, -maximum, 0}, {0, maximum, 0}}
        );
        if(extreme_lines.first != Point3{}
            || extreme_lines.second != Point3{}) return false;
        const long double epsilon =
            1000 * std::numeric_limits<long double>::denorm_min();
        bool unrepresentable_intersection = false;
        try{
            static_cast<void>(closest_points(
                Line3{{0, -1, 0}, {1, -1, 0}},
                Line3{{0, 0, 0}, {1, epsilon, 0}}
            ));
        }catch(const std::overflow_error&){
            unrepresentable_intersection = true;
        }
        if(!unrepresentable_intersection) return false;
        const auto local_segments = closest_points(
            geometry3d_linear_test_segment(),
            Segment3{{0, r, t}, {r, r, t}}
        );
        return geometry3d_linear_test_close(
            local_segments.first, {0, 0, t}
        ) && geometry3d_linear_test_close(
            local_segments.second, {0, r, t}
        );
    });
}
