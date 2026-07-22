// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <tuple>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/3d/query/triangle_bvh_3d.hpp"

namespace{
AABB3 box_of(const Triangle3& triangle){
    return {
        {std::min({triangle.a.x, triangle.b.x, triangle.c.x}),
         std::min({triangle.a.y, triangle.b.y, triangle.c.y}),
         std::min({triangle.a.z, triangle.b.z, triangle.c.z})},
        {std::max({triangle.a.x, triangle.b.x, triangle.c.x}),
         std::max({triangle.a.y, triangle.b.y, triangle.c.y}),
         std::max({triangle.a.z, triangle.b.z, triangle.c.z})},
    };
}

std::optional<std::tuple<std::size_t, long double, Point3>> brute_horizontal(
    const std::vector<Triangle3>& triangles,
    const Ray3& ray
){
    const long double dz = ray.through.z - ray.origin.z;
    std::optional<std::tuple<std::size_t, long double, Point3>> best;
    if(dz == 0) return best;
    for(std::size_t index = 0; index < triangles.size(); ++index){
        const Triangle3& triangle = triangles[index];
        const long double parameter = (triangle.a.z - ray.origin.z) / dz;
        if(parameter < 0) continue;
        const Point3 point{
            ray.origin.x + parameter * (ray.through.x - ray.origin.x),
            ray.origin.y + parameter * (ray.through.y - ray.origin.y),
            triangle.a.z,
        };
        const long double width = triangle.b.x - triangle.a.x;
        const long double height = triangle.c.y - triangle.a.y;
        const long double u = (point.x - triangle.a.x) / width;
        const long double v = (point.y - triangle.a.y) / height;
        if(u < 0 || v < 0 || u + v > 1) continue;
        if(!best || parameter < std::get<1>(*best)
           || (parameter == std::get<1>(*best) && index < std::get<0>(*best))){
            best = std::tuple{index, parameter, point};
        }
    }
    return best;
}
} // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    const std::vector<Triangle3> known{{
        {{0, 0, 5}, {2, 0, 5}, {0, 2, 5}},
        {{0, 0, 2}, {2, 0, 2}, {0, 2, 2}},
        {{0, 0, 2}, {2, 0, 2}, {0, 2, 2}},
    }};
    TriangleBVH3D bvh(known, 1);
    const auto hit = bvh.ray_cast({{0.5L, 0.5L, 0}, {0.5L, 0.5L, 1}});
    if(!hit || std::get<0>(*hit) != 1 || std::get<1>(*hit) != 2
       || !(std::get<2>(*hit) == Point3{0.5L, 0.5L, 2})) return 1;
    if(bvh.ray_cast({{3, 3, 0}, {3, 3, 1}})) return 1;
    if(bvh.aabb_query({{0, 0, 2}, {0, 0, 2}}) != std::vector<std::size_t>({1, 2})) return 1;
    if(bvh.ray_cast({{0.5L, 0.5L, 0}, {0.5L, 0.5L, 1}}, 3, 4)) return 1;
    bool bad_ray = false;
    try{ static_cast<void>(bvh.ray_cast({{0, 0, 0}, {0, 0, 0}})); }
    catch(const std::invalid_argument&){ bad_ray = true; }
    if(!bad_ray) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-100, 100);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        const std::size_t size = static_cast<std::size_t>(random() % 90);
        std::vector<Triangle3> triangles;
        for(std::size_t index = 0; index < size; ++index){
            const long double x = coordinate(random);
            const long double y = coordinate(random);
            const long double z = coordinate(random);
            const long double width = 1 + static_cast<long double>(random() % 20);
            const long double height = 1 + static_cast<long double>(random() % 20);
            triangles.push_back({{x, y, z}, {x + width, y, z}, {x, y + height, z}});
        }
        TriangleBVH3D current(triangles, 3);

        long double x0 = coordinate(random), x1 = coordinate(random);
        long double y0 = coordinate(random), y1 = coordinate(random);
        long double z0 = coordinate(random), z1 = coordinate(random);
        if(x1 < x0) std::swap(x0, x1);
        if(y1 < y0) std::swap(y0, y1);
        if(z1 < z0) std::swap(z0, z1);
        const AABB3 range{{x0, y0, z0}, {x1, y1, z1}};
        std::vector<std::size_t> expected_range;
        for(std::size_t index = 0; index < size; ++index){
            if(box_of(triangles[index]).intersects(range)) expected_range.push_back(index);
        }
        if(current.aabb_query(range) != expected_range) return 1;

        long double ray_x = coordinate(random);
        long double ray_y = coordinate(random);
        if(!triangles.empty() && random() % 2 == 0){
            const Triangle3& target = triangles[random() % triangles.size()];
            ray_x = (target.a.x * 2 + target.b.x + target.c.x) / 4;
            ray_y = (target.a.y * 2 + target.b.y + target.c.y) / 4;
        }
        const Ray3 ray{{ray_x, ray_y, -200}, {ray_x, ray_y, -199}};
        const auto expected_hit = brute_horizontal(triangles, ray);
        const auto actual_hit = current.ray_cast(ray);
        if(static_cast<bool>(expected_hit) != static_cast<bool>(actual_hit)) return 1;
        if(expected_hit){
            if(std::get<0>(*actual_hit) != std::get<0>(*expected_hit)
               || std::abs(std::get<1>(*actual_hit) - std::get<1>(*expected_hit)) > 1e-12L
               || std::hypot(
                   std::get<2>(*actual_hit).x - std::get<2>(*expected_hit).x,
                   std::get<2>(*actual_hit).y - std::get<2>(*expected_hit).y,
                   std::get<2>(*actual_hit).z - std::get<2>(*expected_hit).z
               ) > 1e-10L) return 1;
        }
    }
    std::cout << "OK\n";
    return 0;
}
