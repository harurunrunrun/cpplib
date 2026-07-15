// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/least_squares_plane.hpp"
#include "../../src/algorithm/geometry/3d/dot.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const std::vector<Point3> exact{
            {0, 0, 5}, {1, 0, 7}, {0, 1, 2}, {2, -1, 12}, {-2, 3, -8}
        };
        const auto plane = least_squares_plane(exact);
        const Point3 expected_normal{-2, 3, 1};
        if(!plane || std::abs(dot(plane->normal, expected_normal)) <
           std::hypot(expected_normal.x, expected_normal.y, expected_normal.z) - 1e-9L){
            return false;
        }
        for(const Point3& point: exact){
            if(!geometry3d_api_close(dot(point - plane->point, plane->normal), 0, 1e-8L)){
                return false;
            }
        }
        if(least_squares_plane({{0, 0, 0}, {1, 1, 1}, {2, 2, 2}})) return false;
        if(least_squares_plane({{0, 0, 0}, {1, 0, 0}})) return false;

        std::uniform_real_distribution<long double> coordinate(-10, 10);
        std::normal_distribution<long double> noise(0, 1e-3L);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            for(int index = 0; index < 40; ++index){
                const long double x = coordinate(random);
                const long double y = coordinate(random);
                points.push_back({x, y, 2 * x - 3 * y + 5 + noise(random)});
            }
            const auto fitted = least_squares_plane(points);
            if(!fitted) return false;
            const long double alignment = std::abs(dot(fitted->normal, expected_normal)) /
                std::hypot(expected_normal.x, expected_normal.y, expected_normal.z);
            if(alignment < 0.99999L) return false;
        }
        return true;
    });
}
