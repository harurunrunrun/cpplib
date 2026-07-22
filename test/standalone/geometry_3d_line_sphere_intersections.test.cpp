// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point_collection/line_sphere_intersections.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)line_sphere_intersections(Line3{{-2, 0, 0}, {2, 0, 0}}, sphere);
    })) return 1;
    return geometry3d_test_main([](){
        const auto points = line_sphere_intersections(
            {{-3, 0, 0}, {3, 0, 0}}, {{0, 0, 0}, 2}
        );
        if(points.size() != 2 ||
            !geometry3d_close_point(points[0], {-2, 0, 0}) ||
            !geometry3d_close_point(points[1], {2, 0, 0})) return false;

        const long double huge = std::ldexp(1.0L, 1000);
        const long double ulp = std::nextafter(
            huge, std::numeric_limits<long double>::infinity()
        ) - huge;
        const long double radius = 64 * ulp;
        const auto translated = line_sphere_intersections(
            {{huge - 2 * radius, huge, huge},
             {huge + 2 * radius, huge, huge}},
            {{huge, huge, huge}, radius}
        );
        if(translated.size() != 2 || translated[0].x != huge - radius ||
            translated[1].x != huge + radius) return false;

        const long double tiny = 1e-3000L;
        const auto microscopic = line_sphere_intersections(
            {{-2 * tiny, 0, 0}, {2 * tiny, 0, 0}}, {{0, 0, 0}, tiny}
        );
        return microscopic.size() == 2 &&
            std::abs(microscopic[0].x / tiny + 1.0L) < 1e-12L &&
            std::abs(microscopic[1].x / tiny - 1.0L) < 1e-12L;
    });
}
