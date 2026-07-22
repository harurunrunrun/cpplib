// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/spherical_triangle_area.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!geometry3d_api_close(
                spherical_triangle_area({1, 0, 0}, {0, 1, 0}, {0, 0, 1}),
                GEOMETRY3D_PI / 2
            ) || !geometry3d_api_close(
                spherical_triangle_area({0, 0, 1}, {0, 1, 0}, {1, 0, 0}, 2),
                2 * GEOMETRY3D_PI
            ) || spherical_triangle_area({1, 0, 0}, {1, 0, 0}, {0, 1, 0}) != 0.0L){
            return false;
        }
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const long double a = static_cast<long double>(random() % 1000 + 1) / 100.0L;
            const long double b = static_cast<long double>(random() % 1000 + 1) / 100.0L;
            const long double c = static_cast<long double>(random() % 1000 + 1) / 100.0L;
            const long double area = spherical_triangle_area({a, 0, 0}, {0, b, 0}, {0, 0, c});
            if(!geometry3d_api_close(area, GEOMETRY3D_PI / 2)) return false;
        }
        return true;
    });
}
