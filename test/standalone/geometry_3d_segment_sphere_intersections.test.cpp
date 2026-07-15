// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/segment_sphere_intersections.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)segment_sphere_intersections(Segment3{{0, 0, 0}, {0, 0, 0}}, sphere);
    })) return 1;
    const long double infinity = std::numeric_limits<long double>::infinity();
    if(!geometry3d_throws<std::invalid_argument>([&]{
        (void)segment_sphere_intersections(
            Segment3{{infinity, 0, 0}, {infinity, 0, 0}}, Sphere3{{0, 0, 0}, 1}
        );
    })) return 1;
    return geometry3d_test_main([](){
        const auto points = segment_sphere_intersections(
            {{-3, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, 2}
        );
        if(points.size() != 1 ||
            !geometry3d_close_point(points[0], {-2, 0, 0})) return false;

        const long double maximum =
            std::numeric_limits<long double>::max();
        const auto one_representable_root = segment_sphere_intersections(
            {{0, 0, 0}, {1, 0, 0}},
            {{maximum * 0.75L, 0, 0}, maximum * 0.75L}
        );
        return one_representable_root.size() == 1 &&
            one_representable_root.front().x == 0.0L;
    });
}
