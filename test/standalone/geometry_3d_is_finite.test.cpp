// competitive-verifier: STANDALONE

#include <limits>

#include "../../src/algorithm/geometry/3d/is_finite.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_circles([](const Circle3& circle){
        geometry3d_validate(circle);
    }) || !geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        geometry3d_validate(sphere);
    })) return 1;
    const long double huge = std::ldexp(1.0L, 1000);
    const long double ulp = std::nextafter(
        huge, std::numeric_limits<long double>::infinity()
    ) - huge;
    if(geometry3d_safe_normalized_difference(
        huge + 64 * ulp, huge, huge + 64 * ulp
    ) <= 0.0L || geometry3d_scaled_sign(1e-3000L, 1e-3000L) != 1){
        return 1;
    }
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const long double infinity = std::numeric_limits<long double>::infinity();
        const long double nan = std::numeric_limits<long double>::quiet_NaN();
        return geometry3d_is_finite(0.0L)
            && geometry3d_is_finite(Point3{1, 2, 3})
            && geometry3d_is_finite(Line3{{0, 0, 0}, {1, 1, 1}})
            && geometry3d_is_finite(Ray3{{0, 0, 0}, {1, 1, 1}})
            && geometry3d_is_finite(Segment3{{0, 0, 0}, {1, 1, 1}})
            && geometry3d_is_finite(Plane3{{0, 0, 0}, {0, 0, 1}})
            && geometry3d_is_finite(Circle3{{0, 0, 0}, {0, 0, 1}, 2})
            && geometry3d_is_finite(Sphere3{{0, 0, 0}, 2})
            && geometry3d_is_finite(Triangle3{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}})
            && geometry3d_is_finite(Tetrahedron3{
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
            })
            && !geometry3d_is_finite(infinity)
            && !geometry3d_is_finite(nan)
            && !geometry3d_is_finite(Point3{0, infinity, 0});
    });
}
