// competitive-verifier: STANDALONE

#include <limits>

#include "../../src/algorithm/geometry/3d/shape/convex_polyhedron_plane_section.hpp"
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
        const long double translation = 1e3000L;
        const long double upper = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        );
        std::vector<Point3> vertices;
        for(int x = 0; x < 2; ++x){
            for(int y = 0; y < 2; ++y){
                for(int z = 0; z < 2; ++z){
                    vertices.push_back({
                        static_cast<long double>(x),
                        static_cast<long double>(y),
                        z ? upper : translation,
                    });
                }
            }
        }
        const Polygon3 thin = convex_polyhedron_plane_section(
            convex_hull_3d(vertices), {{0.5L, 0, 0}, {1, 0, 0}}
        );
        if(thin.size() != 4) return false;
        for(const Point3& point: thin){
            if(point.x != 0.5L
                || (point.z != translation && point.z != upper)) return false;
        }
        return convex_polyhedron_plane_section(
            geometry3d_test_cube(), {{0, 0, 3}, {0, 0, 1}}
        ).empty();
    });
}
