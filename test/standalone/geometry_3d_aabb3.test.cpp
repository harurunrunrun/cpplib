// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>

#include "../../src/algorithm/geometry/3d/aabb3.hpp"

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    const AABB3 unit{{0, 0, 0}, {1, 2, 3}};
    if(!unit.valid() || !unit.contains({0, 2, 1}) || unit.contains({-1, 1, 1})) return 1;
    if(std::abs(unit.distance({4, 6, 3}) - 5) > 1e-14L) return 1;
    if(!unit.intersects({{1, 2, 3}, {4, 5, 6}})
       || unit.intersects({{2, 0, 0}, {3, 1, 1}})) return 1;

    bool invalid_threw = false;
    try{
        static_cast<void>(AABB3{{1, 0, 0}, {0, 1, 1}}.distance({0, 0, 0}));
    }catch(const std::invalid_argument&){ invalid_threw = true; }
    if(!invalid_threw) return 1;
    bool nonfinite_threw = false;
    try{
        static_cast<void>(unit.contains({std::numeric_limits<long double>::infinity(), 0, 0}));
    }catch(const std::invalid_argument&){ nonfinite_threw = true; }
    if(!nonfinite_threw) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-1000, 1000);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        long double x0 = coordinate(random), x1 = coordinate(random);
        long double y0 = coordinate(random), y1 = coordinate(random);
        long double z0 = coordinate(random), z1 = coordinate(random);
        if(x1 < x0) std::swap(x0, x1);
        if(y1 < y0) std::swap(y0, y1);
        if(z1 < z0) std::swap(z0, z1);
        const AABB3 box{{x0, y0, z0}, {x1, y1, z1}};
        const Point3 point{
            static_cast<long double>(coordinate(random)),
            static_cast<long double>(coordinate(random)),
            static_cast<long double>(coordinate(random)),
        };
        const bool expected_contains = x0 <= point.x && point.x <= x1
            && y0 <= point.y && point.y <= y1 && z0 <= point.z && point.z <= z1;
        const long double dx = std::max({x0 - point.x, 0.0L, point.x - x1});
        const long double dy = std::max({y0 - point.y, 0.0L, point.y - y1});
        const long double dz = std::max({z0 - point.z, 0.0L, point.z - z1});
        if(box.contains(point) != expected_contains
           || box.distance(point) != std::hypot(dx, dy, dz)) return 1;
    }
    std::cout << "OK\n";
    return 0;
}
