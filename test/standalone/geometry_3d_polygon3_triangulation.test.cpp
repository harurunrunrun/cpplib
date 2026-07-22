// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/polygon3_triangulation.hpp"
#include "../../src/algorithm/geometry/3d/scalar/area.hpp"
#include "../../src/algorithm/geometry/3d/scalar/polygon3_area.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{{0, 0, 2}, {3, 0, 2}, {3, 1, 2}, {1, 1, 2}, {1, 3, 2}, {0, 3, 2}};
        const auto triangles = polygon3_triangulation(polygon);
        long double sum = 0;
        for(const Triangle3& triangle: triangles) sum += area(triangle);
        return triangles.size() == polygon.size() - 2
            && geometry3d_api_close(sum, polygon3_area(polygon));
    });
}
