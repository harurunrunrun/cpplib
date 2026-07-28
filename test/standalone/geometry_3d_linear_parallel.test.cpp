// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/line_line/line_line_parallel.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/line_ray_parallel.hpp"
#include "../../src/algorithm/geometry/3d/predicate/line_ray/line_ray_parallel_line_ray.hpp"
#include "../../src/algorithm/geometry/3d/predicate/ray_line/line_ray_parallel_ray_line.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/line_segment_parallel.hpp"
#include "../../src/algorithm/geometry/3d/predicate/line_segment/line_segment_parallel_line_segment.hpp"
#include "../../src/algorithm/geometry/3d/predicate/segment_line/line_segment_parallel_segment_line.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/linear_parallel.hpp"
#include "../../src/algorithm/geometry/3d/predicate/ray_ray/ray_ray_parallel.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/ray_segment_parallel.hpp"
#include "../../src/algorithm/geometry/3d/predicate/ray_segment/ray_segment_parallel_ray_segment.hpp"
#include "../../src/algorithm/geometry/3d/predicate/segment_ray/ray_segment_parallel_segment_ray.hpp"
#include "../../src/algorithm/geometry/3d/predicate/segment_segment/segment_segment_parallel.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Line3 line{{}, {1, 0, 0}};
        const Line3 line_parallel{{0, 1, 0}, {-2, 1, 0}};
        const Ray3 ray{{0, 2, 0}, {-2, 2, 0}};
        const Segment3 segment{{0, 0, 1}, {4, 0, 1}};
        const Ray3 transverse_ray{{}, {0, 1, 0}};
        const Segment3 transverse_segment{{}, {0, 1, 0}};
        if(!parallel(line, line_parallel)
            || !parallel(line, ray) || !parallel(ray, line)
            || !parallel(line, segment) || !parallel(segment, line)
            || !parallel(ray, ray)
            || !parallel(ray, segment) || !parallel(segment, ray)
            || !parallel(segment, segment)
            || parallel(line, transverse_ray)
            || parallel(transverse_ray, line)
            || parallel(line, transverse_segment)
            || parallel(transverse_segment, line)
            || parallel(ray, transverse_segment)
            || parallel(transverse_segment, ray)
            || parallel(line, Line3{{}, {0, 1, 0}})){
            return false;
        }

        try{
            (void)parallel(line, Segment3{{}, {}});
            return false;
        }catch(const std::invalid_argument&){
        }
        try{
            (void)parallel(Segment3{{}, {}}, line);
            return false;
        }catch(const std::invalid_argument&){
        }
        try{
            (void)parallel(Ray3{{}, {}}, line);
            return false;
        }catch(const std::invalid_argument&){
        }
        return true;
    });
}