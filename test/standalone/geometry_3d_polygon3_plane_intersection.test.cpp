// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/query/polygon3_plane_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{
            {-3, -2, 0}, {3, -2, 0}, {3, 2, 0}, {1, 2, 0},
            {1, 0, 0}, {-1, 0, 0}, {-1, 2, 0}, {-3, 2, 0},
        };
        const auto cut = polygon3_plane_intersection(
            polygon, {{0, 0, 0}, {1, 0, 0}}
        );
        const auto coplanar = polygon3_plane_intersection(
            polygon, {{0, 0, 0}, {0, 0, 1}}
        );
        return cut.segments.size() == 1 && !cut.coplanar_polygon
            && coplanar.coplanar_polygon
            && coplanar.coplanar_polygon->size() == polygon.size();
    });
}
