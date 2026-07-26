// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_polyhedron_support_point.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Point3 point = convex_polyhedron_support_point(
            geometry3d_test_cube(), {1, 0, 0}
        );
        if(point.x != 1.0L) return false;
        const long double translation = 1e3000L;
        const long double local = 1e-3000L;
        const ConvexPolyhedron3 thin{
            1, {{0, 0, translation}, {local, 0, translation}}, {}
        };
        const Point3 thin_support = convex_polyhedron_support_point(
            thin, {1, 0, 0}
        );
        if(thin_support.x != local || thin_support.z != translation){
            return false;
        }
        try{ (void)convex_polyhedron_support_point(geometry3d_test_cube(), {}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
