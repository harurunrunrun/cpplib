// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_polyhedron_plane_section.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 section = convex_polyhedron_plane_section(
            geometry3d_test_cube(), {{0, 0, 0}, {0, 0, 1}}
        );
        if(section.size() != 4) return false;
        for(const Point3& point: section){
            if(!geometry3d_convex_close(point.z, 0.0L)) return false;
        }
        return convex_polyhedron_plane_section(
            geometry3d_test_cube(), {{0, 0, 3}, {0, 0, 1}}
        ).empty();
    });
}
