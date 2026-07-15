// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_relation.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    const Sphere3 valid_sphere{{0, 0, 0}, 1};
    if(!geometry3d_rejects_invalid_spheres([&](const Sphere3& sphere){
        (void)sphere_relation(sphere, valid_sphere);
    }) || !geometry3d_rejects_invalid_spheres([&](const Sphere3& sphere){
        (void)sphere_relation(valid_sphere, sphere);
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 base{{}, 3};
        if(sphere_relation(base, base) != SPHERE_COINCIDENT) return false;
        if(sphere_relation(base, {{7, 0, 0}, 3}) != SPHERE_SEPARATE ||
            sphere_relation(base, {{6, 0, 0}, 3}) != SPHERE_EXTERNALLY_TANGENT ||
            sphere_relation(base, {{4, 0, 0}, 3}) != SPHERE_INTERSECT ||
            sphere_relation(base, {{2, 0, 0}, 1}) != SPHERE_INTERNALLY_TANGENT ||
            sphere_relation(base, {{1, 0, 0}, 1}) != SPHERE_INSIDE) return false;

        const long double huge = std::ldexp(1.0L, 1000);
        const long double ulp = std::nextafter(
            huge, std::numeric_limits<long double>::infinity()
        ) - huge;
        const long double radius = 64 * ulp;
        if(sphere_relation(
            {{huge, huge, huge}, radius},
            {{huge + radius, huge, huge}, radius}
        ) != SPHERE_INTERSECT) return false;
        const long double tiny = 1e-3000L;
        if(sphere_relation(
            {{0, 0, 1e3000L}, 2 * tiny},
            {{0, 0, 1e3000L}, tiny}
        ) != SPHERE_INSIDE) return false;
        return sphere_relation(
            {{0, 0, 0}, tiny}, {{tiny, 0, 0}, tiny}
        ) == SPHERE_INTERSECT;
    });
}
