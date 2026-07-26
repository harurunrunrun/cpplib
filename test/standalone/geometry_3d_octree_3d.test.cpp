// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/3d/octree_3d.hpp"

namespace{
long double point_distance(const Point3& left, const Point3& right){
    return std::hypot(left.x - right.x, left.y - right.y, left.z - right.z);
}

std::optional<std::pair<std::size_t, long double>> brute_nearest(
    const std::vector<Point3>& points,
    const Point3& query
){
    if(points.empty()) return std::nullopt;
    std::pair<std::size_t, long double> best{0, point_distance(points[0], query)};
    for(std::size_t index = 1; index < points.size(); ++index){
        const long double current = point_distance(points[index], query);
        if(current < best.second || (current == best.second && index < best.first)){
            best = {index, current};
        }
    }
    return best;
}
} // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    Octree3D tree({{-10, -10, -10}, {10, 10, 10}}, 1, 8);
    if(tree.nearest({0, 0, 0}) || tree.insert({0, 0, 0}) != 0
       || tree.insert({0, 0, 0}) != 1 || tree.insert({10, 10, 10}) != 2) return 1;
    const auto tied = tree.nearest({1, 0, 0});
    if(!tied || tied->first != 0 || tied->second != 1) return 1;
    if(tree.range_query({{0, 0, 0}, {10, 10, 10}}) != std::vector<std::size_t>({0, 1, 2})) return 1;
    bool outside_threw = false;
    try{ static_cast<void>(tree.insert({11, 0, 0})); }
    catch(const std::invalid_argument&){ outside_threw = true; }
    if(!outside_threw) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-1000, 1000);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        Octree3D current({{-1000, -1000, -1000}, {1000, 1000, 1000}}, 4, 20);
        const std::size_t size = static_cast<std::size_t>(random() % 100);
        std::vector<Point3> points;
        for(std::size_t index = 0; index < size; ++index){
            points.push_back({
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
            });
            if(index && random() % 19 == 0) points.back() = points[random() % index];
            if(current.insert(points.back()) != index) return 1;
        }
        const Point3 query{
            static_cast<long double>(coordinate(random) * 2),
            static_cast<long double>(coordinate(random) * 2),
            static_cast<long double>(coordinate(random) * 2),
        };
        if(current.nearest(query) != brute_nearest(points, query)) return 1;

        long double x0 = coordinate(random), x1 = coordinate(random);
        long double y0 = coordinate(random), y1 = coordinate(random);
        long double z0 = coordinate(random), z1 = coordinate(random);
        if(x1 < x0) std::swap(x0, x1);
        if(y1 < y0) std::swap(y0, y1);
        if(z1 < z0) std::swap(z0, z1);
        const AABB3 range{{x0, y0, z0}, {x1, y1, z1}};
        std::vector<std::size_t> expected;
        for(std::size_t index = 0; index < points.size(); ++index){
            if(range.contains(points[index])) expected.push_back(index);
        }
        if(current.range_query(range) != expected) return 1;
    }
    std::cout << "OK\n";
    return 0;
}
