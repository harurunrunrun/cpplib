// competitive-verifier: STANDALONE

#include <limits>

#include "../../src/algorithm/geometry/3d/checked_nonnegative_product.hpp"
#include "../../src/algorithm/geometry/3d/circle3_is_finite.hpp"
#include "../../src/algorithm/geometry/3d/circle3_validate.hpp"
#include "../../src/algorithm/geometry/3d/line3_is_finite.hpp"
#include "../../src/algorithm/geometry/3d/long_double_is_finite.hpp"
#include "../../src/algorithm/geometry/3d/long_double_safe_normalized_difference.hpp"
#include "../../src/algorithm/geometry/3d/long_double_scaled_sign.hpp"
#include "../../src/algorithm/geometry/3d/nonnegative_scaled_comparison.hpp"
#include "../../src/algorithm/geometry/3d/plane3_is_finite.hpp"
#include "../../src/algorithm/geometry/3d/point3_is_finite.hpp"
#include "../../src/algorithm/geometry/3d/point3_normalized_difference_points.hpp"
#include "../../src/algorithm/geometry/3d/point3_normalized_difference_result.hpp"
#include "../../src/algorithm/geometry/3d/point3_normalized_difference.hpp"
#include "../../src/algorithm/geometry/3d/point3_safe_normalized_difference.hpp"
#include "../../src/algorithm/geometry/3d/ray3_is_finite.hpp"
#include "../../src/algorithm/geometry/3d/segment3_is_finite.hpp"
#include "../../src/algorithm/geometry/3d/sphere3_is_finite.hpp"
#include "../../src/algorithm/geometry/3d/sphere3_validate.hpp"
#include "../../src/algorithm/geometry/3d/tetrahedron3_is_finite.hpp"
#include "../../src/algorithm/geometry/3d/triangle3_is_finite.hpp"
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
    const Point3 normalized = geometry3d_safe_normalized_difference(
        Point3{huge + 64 * ulp, 2, 3}, Point3{huge, 1, 1},
        huge + 64 * ulp
    );
    const auto difference = geometry3d_normalized_difference(
        Point3{3, 4, 0}, Point3{0, 0, 0}, {10}
    );
    if(normalized.x <= 0.0L
        || difference.scale != 10.0L
        || std::abs(difference.value.x - 0.3L) > 1e-18L
        || geometry3d_compare_nonnegative_scaled(2, 3, 1, 5) != 1
        || geometry3d_detail::checked_nonnegative_product(
            {2, 3}, "unexpected overflow"
        ) != 6.0L){
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
