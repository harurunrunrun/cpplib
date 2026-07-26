// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/line_line_overlap.hpp"
#include "../../src/algorithm/geometry/3d/line_ray_overlap.hpp"
#include "../../src/algorithm/geometry/3d/line_ray_overlap_line_ray.hpp"
#include "../../src/algorithm/geometry/3d/line_ray_overlap_ray_line.hpp"
#include "../../src/algorithm/geometry/3d/line_segment_overlap.hpp"
#include "../../src/algorithm/geometry/3d/line_segment_overlap_line_segment.hpp"
#include "../../src/algorithm/geometry/3d/line_segment_overlap_segment_line.hpp"
#include "../../src/algorithm/geometry/3d/linear_overlap.hpp"
#include "../../src/algorithm/geometry/3d/ray_ray_overlap.hpp"
#include "../../src/algorithm/geometry/3d/ray_segment_overlap.hpp"
#include "../../src/algorithm/geometry/3d/ray_segment_overlap_ray_segment.hpp"
#include "../../src/algorithm/geometry/3d/ray_segment_overlap_segment_ray.hpp"
#include "../../src/algorithm/geometry/3d/segment_segment_overlap.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Line3 line{{}, {1, 0, 0}};
        const Line3 same_line{{2, 0, 0}, {3, 0, 0}};
        const Ray3 ray{{}, {1, 0, 0}};
        const Segment3 segment{{-1, 0, 0}, {2, 0, 0}};
        const Ray3 separated_ray{{0, 1, 0}, {1, 1, 0}};
        const Segment3 separated_segment{{0, 1, 0}, {1, 1, 0}};
        return overlap(line, same_line)
            && overlap(line, ray) && overlap(ray, line)
            && overlap(line, segment) && overlap(segment, line)
            && overlap(ray, Ray3{{2, 0, 0}, {1, 0, 0}})
            && overlap(ray, segment) && overlap(segment, ray)
            && overlap(segment, Segment3{{1, 0, 0}, {4, 0, 0}})
            && !overlap(line, separated_ray)
            && !overlap(separated_ray, line)
            && !overlap(line, separated_segment)
            && !overlap(separated_segment, line)
            && !overlap(ray, separated_segment)
            && !overlap(separated_segment, ray)
            && !overlap(
                Segment3{{}, {1, 0, 0}},
                Segment3{{1, 0, 0}, {2, 0, 0}}
            );
    });
}