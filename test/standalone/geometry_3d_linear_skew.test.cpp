// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/line_line_skew.hpp"
#include "../../src/algorithm/geometry/3d/line_ray_skew.hpp"
#include "../../src/algorithm/geometry/3d/line_ray_skew_line_ray.hpp"
#include "../../src/algorithm/geometry/3d/line_ray_skew_ray_line.hpp"
#include "../../src/algorithm/geometry/3d/line_segment_skew.hpp"
#include "../../src/algorithm/geometry/3d/line_segment_skew_line_segment.hpp"
#include "../../src/algorithm/geometry/3d/line_segment_skew_segment_line.hpp"
#include "../../src/algorithm/geometry/3d/linear_skew.hpp"
#include "../../src/algorithm/geometry/3d/ray_ray_skew.hpp"
#include "../../src/algorithm/geometry/3d/ray_segment_skew.hpp"
#include "../../src/algorithm/geometry/3d/ray_segment_skew_ray_segment.hpp"
#include "../../src/algorithm/geometry/3d/ray_segment_skew_segment_ray.hpp"
#include "../../src/algorithm/geometry/3d/segment_segment_skew.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Line3 x_line{{}, {1, 0, 0}};
        const Line3 y_line{{0, 1, 1}, {0, 2, 1}};
        const Ray3 x_ray{{}, {1, 0, 0}};
        const Ray3 y_ray{{0, 1, 1}, {0, 2, 1}};
        const Segment3 x_segment{{}, {1, 0, 0}};
        const Segment3 y_segment{{0, 1, 1}, {0, 2, 1}};
        const Ray3 coplanar_ray{{0, -1, 0}, {0, 1, 0}};
        const Segment3 coplanar_segment{{0, -1, 0}, {0, 1, 0}};
        return skew(x_line, y_line)
            && skew(x_line, y_ray) && skew(y_ray, x_line)
            && skew(x_line, y_segment) && skew(y_segment, x_line)
            && skew(x_ray, y_ray)
            && skew(x_ray, y_segment) && skew(y_segment, x_ray)
            && skew(x_segment, y_segment)
            && !skew(x_line, coplanar_ray)
            && !skew(coplanar_ray, x_line)
            && !skew(x_line, coplanar_segment)
            && !skew(coplanar_segment, x_line)
            && !skew(x_ray, coplanar_segment)
            && !skew(coplanar_segment, x_ray)
            && !skew(x_line, Line3{{0, -1, 0}, {0, 1, 0}})
            && !skew(x_line, Line3{{0, 1, 0}, {1, 1, 0}});
    });
}