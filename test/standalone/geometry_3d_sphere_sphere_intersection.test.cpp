// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/sphere_sphere_intersection.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    const Sphere3 valid_sphere{{0, 0, 0}, 1};
    if(!geometry3d_rejects_invalid_spheres([&](const Sphere3& sphere){
        (void)sphere_sphere_intersection(sphere, valid_sphere);
    }) || !geometry3d_rejects_invalid_spheres([&](const Sphere3& sphere){
        (void)sphere_sphere_intersection(valid_sphere, sphere);
    })) return 1;
    return geometry3d_test_main([](){
        const auto circle = sphere_sphere_intersection(
            {{0, 0, 0}, 2}, {{2, 0, 0}, 2}
        );
        if(!circle || !geometry3d_close_point(circle->center, {1, 0, 0}) ||
            !geometry3d_close_value(circle->radius, std::sqrt(3.0L))){
            return false;
        }
        const long double huge = std::ldexp(1.0L, 1000);
        const long double ulp = std::nextafter(
            huge, std::numeric_limits<long double>::infinity()
        ) - huge;
        const long double radius = 64 * ulp;
        const auto translated = sphere_sphere_intersection(
            {{huge, huge, huge}, radius},
            {{huge + radius, huge, huge}, radius}
        );
        if(!translated || translated->radius <= 0.0L ||
            translated->center.x != huge + radius / 2.0L) return false;

        const long double tiny = 1e-3000L;
        const auto microscopic = sphere_sphere_intersection(
            {{0, 0, 0}, tiny}, {{tiny, 0, 0}, tiny}
        );
        return microscopic && microscopic->radius > 0.0L &&
            std::abs(
                microscopic->radius / tiny - std::sqrt(3.0L) / 2.0L
            ) < 1e-12L;
    });
}
