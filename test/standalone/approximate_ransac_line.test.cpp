// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/geometry/ransac_line.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t n;
    long double threshold;
    std::size_t iterations;
    std::uint64_t seed;
    std::size_t expected;
    if(!(std::cin >> n >> threshold >> iterations >> seed >> expected)) return 0;
    std::vector<std::array<long double, 2>> points(n);
    for(auto& point: points) std::cin >> point[0] >> point[1];
    std::mt19937_64 first_random(seed);
    std::mt19937_64 second_random(seed);
    const auto first = approximate::geometry::ransac_line(
        points, threshold, iterations, first_random
    );
    const auto second = approximate::geometry::ransac_line(
        points, threshold, iterations, second_random
    );
    assert(first.found == second.found);
    assert(first.point == second.point);
    assert(first.direction == second.direction);
    assert(first.inliers == second.inliers);
    assert(first.inliers.size() == expected);
    if(first.found){
        assert(std::fabs(std::hypot(first.direction[0], first.direction[1]) - 1.0L) < 1.0e-15L);
        for(std::size_t index: first.inliers){
            const long double dx = points[index][0] - first.point[0];
            const long double dy = points[index][1] - first.point[1];
            const long double distance = std::fabs(dx * first.direction[1] - dy * first.direction[0]);
            assert(distance <= threshold + 1.0e-15L);
        }
    }
    bool invalid = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::geometry::ransac_line(points, threshold, 0, random));
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::geometry::ransac_line(
            std::vector<std::array<double, 2>>{{0.0, 0.0},
                {std::numeric_limits<double>::quiet_NaN(), 1.0}},
            1.0L, 1, random
        ));
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    std::cout << first.inliers.size() << '\n';
}
