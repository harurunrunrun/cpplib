// competitive-verifier: STANDALONE

#include <limits>

#include "../../src/algorithm/geometry/3d/is_finite.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
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
