// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point_collection/circle_circle_intersections.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    const Circle3 valid_circle{{0, 0, 0}, {0, 0, 1}, 1};
    if(!geometry3d_rejects_invalid_circles([&](const Circle3& circle){
        (void)circle_circle_intersections(circle, valid_circle);
    }) || !geometry3d_rejects_invalid_circles([&](const Circle3& circle){
        (void)circle_circle_intersections(valid_circle, circle);
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 first{{}, {0, 0, 1}, 1};
        const auto coplanar = circle_circle_intersections(first, {{1, 0, 0}, {0, 0, -2}, 1});
        const auto tangent = circle_circle_intersections(first, {{2, 0, 0}, {0, 0, 1}, 1});
        const auto noncoplanar = circle_circle_intersections(first, {{}, {1, 0, 0}, 1});
        if(coplanar.size() != 2 || tangent.size() != 1 || noncoplanar.size() != 2) return false;
        for(const Point3& point: coplanar){
            if(!geometry3d_api_close(point.x, 0.5L) ||
                !geometry3d_api_close(abs(point), 1)) return false;
        }
        const long double huge = std::ldexp(1.0L, 1000);
        const long double ulp = std::nextafter(
            huge, std::numeric_limits<long double>::infinity()
        ) - huge;
        const long double radius = 64 * ulp;
        const Circle3 translated_first{
            {huge, huge, huge}, {0, 0, 1}, radius
        };
        const Circle3 translated_second{
            {huge + radius, huge, huge}, {0, 0, 1}, radius
        };
        const auto translated = circle_circle_intersections(
            translated_first, translated_second
        );
        const long double expected_x = huge + radius / 2.0L;
        if(translated.size() != 2 ||
            std::abs(translated[0].x - expected_x) > 2 * ulp ||
            std::abs(translated[1].x - expected_x) > 2 * ulp ||
            !geometry3d_is_finite(translated[0]) ||
            !geometry3d_is_finite(translated[1]) ||
            translated[0].y >= huge || translated[1].y <= huge) return false;

        const long double tiny = 1e-3000L;
        const Circle3 microscopic_first{{0, 0, 0}, {0, 0, 1}, tiny};
        const Circle3 microscopic_second{{tiny, 0, 0}, {0, 0, 1}, tiny};
        const auto microscopic = circle_circle_intersections(
            microscopic_first, microscopic_second
        );
        if(microscopic.size() != 2) return false;
        try{ (void)circle_circle_intersections(first, first); }
        catch(const std::domain_error&){ return true; }
        return false;
    });
}
