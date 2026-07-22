// competitive-verifier: STANDALONE

#include <algorithm>
#include <vector>

#include "../../src/algorithm/geometry/3d/scalar/spherical_polygon_area.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        std::vector<Point3> triangle{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        if(!geometry3d_api_close(spherical_polygon_area(triangle), GEOMETRY3D_PI / 2)
            || spherical_polygon_area({{1, 0, 0}, {0, 1, 0}}) != 0.0L) return false;
        std::reverse(triangle.begin(), triangle.end());
        if(!geometry3d_api_close(spherical_polygon_area(triangle, 3), 4.5L * GEOMETRY3D_PI)){
            return false;
        }
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const long double scale = static_cast<long double>(random() % 1000 + 1);
            std::vector<Point3> scaled{{scale, 0, 0}, {0, 2 * scale, 0}, {0, 0, 3 * scale}};
            if(!geometry3d_api_close(spherical_polygon_area(scaled), GEOMETRY3D_PI / 2)){
                return false;
            }
        }
        return true;
    });
}
