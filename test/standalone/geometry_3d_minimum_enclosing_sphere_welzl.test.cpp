// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/minimum_enclosing_sphere_welzl.hpp"

namespace{

bool close_value(long double left, long double right){
    return std::abs(left - right) <= 3e-8L *
        std::max({1.0L, std::abs(left), std::abs(right)});
}

bool contains_all(const Sphere3& sphere, const std::vector<Point3>& points){
    for(const Point3& point: points){
        if(std::hypot(
            point.x - sphere.center.x,
            point.y - sphere.center.y,
            point.z - sphere.center.z
        ) > sphere.radius + 3e-8L * std::max(1.0L, sphere.radius)) return false;
    }
    return true;
}

} // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    const std::vector<Point3> degenerate{{
        {1, 2, 3}, {1, 2, 3}, {5, 2, 3}, {9, 2, 3}, {3, 2, 3},
    }};
    const Sphere3 line = minimum_enclosing_sphere_welzl(degenerate, seed);
    if(!close_value(line.radius, 4) || !contains_all(line, degenerate)) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-50, 50);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        const std::size_t size = 1 + static_cast<std::size_t>(random() % 24);
        std::vector<Point3> points;
        points.reserve(size);
        for(std::size_t index = 0; index < size; ++index){
            points.push_back({
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
            });
            if(index > 0 && random() % 9 == 0) points.back() = points[index - 1];
        }
        const Sphere3 recursive = minimum_enclosing_sphere_welzl(points, random());
        const Sphere3 iterative = minimum_enclosing_sphere(points, random());
        if(!contains_all(recursive, points) ||
           !close_value(recursive.radius, iterative.radius)) return 1;
    }
    std::cout << "OK\n";
    return 0;
}
