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

#include "../../src/algorithm/geometry/3d/scalar/point_set_diameter_3d.hpp"

namespace{

PointSetDiameter3DResult brute(const std::vector<Point3>& points){
    PointSetDiameter3DResult result{0, 1, 0};
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1; second < points.size(); ++second){
            const long double distance =
                point_set_diameter_3d_detail::scaled_euclidean_distance(
                    points[first], points[second]
                );
            if(distance > result.distance ||
               (distance == result.distance &&
                std::pair{first, second} < std::pair{result.first, result.second})){
                result = {first, second, distance};
            }
        }
    }
    return result;
}

} // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;
    if(point_set_diameter_3d({}) ||
       point_set_diameter_3d({{1, 2, 3}}) ||
       point_set_diameter_3d_aabb_branch_and_bound({}) ||
       point_set_diameter_3d_aabb_branch_and_bound({{1, 2, 3}})) return 1;

    const std::vector<Point3> points{{
        {0, 0, 0}, {2, 0, 0}, {-2, 0, 0}, {0, 2, 0}, {0, 0, 2},
    }};
    const auto result = point_set_diameter_3d(points);
    const auto bounded_result =
        point_set_diameter_3d_aabb_branch_and_bound(points);
    if(!result || result->first != 1 || result->second != 2 ||
       std::abs(result->distance - 4) > 1e-12L) return 1;
    if(!bounded_result || bounded_result->first != result->first ||
       bounded_result->second != result->second ||
       bounded_result->distance != result->distance) return 1;

    const std::vector<Point3> tied{{
        {-1, -1, 0}, {1, 1, 0}, {-1, 1, 0}, {1, -1, 0},
        {-1, -1, 0}, {0, 0, 0},
    }};
    const auto tied_result =
        point_set_diameter_3d_aabb_branch_and_bound(tied);
    if(!tied_result || tied_result->first != 0 || tied_result->second != 1)
        return 1;

    const long double maximum =
        std::numeric_limits<long double>::max();
    const std::vector<Point3> overflowing{{
        {maximum / 2, 0, 0}, {-maximum, 0, 0},
        {maximum, 0, 0}, {0, 0, 0},
    }};
    const PointSetDiameter3DResult overflowing_expected = brute(overflowing);
    const auto overflowing_plain = point_set_diameter_3d(overflowing);
    const auto overflowing_bounded =
        point_set_diameter_3d_aabb_branch_and_bound(overflowing);
    if(overflowing_expected.first != 0 ||
       overflowing_expected.second != 1 ||
       !std::isinf(overflowing_expected.distance) ||
       !overflowing_plain || !overflowing_bounded ||
       overflowing_plain->first != overflowing_expected.first ||
       overflowing_plain->second != overflowing_expected.second ||
       overflowing_plain->distance != overflowing_expected.distance ||
       overflowing_bounded->first != overflowing_expected.first ||
       overflowing_bounded->second != overflowing_expected.second ||
       overflowing_bounded->distance != overflowing_expected.distance) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-100, 100);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        const std::size_t size = 2 + static_cast<std::size_t>(random() % 90);
        std::vector<Point3> current;
        current.reserve(size);
        const long double shift = iteration % 2 == 0 ? 1e14L : 0;
        for(std::size_t index = 0; index < size; ++index){
            current.push_back({
                shift + coordinate(random),
                -shift + coordinate(random),
                shift + coordinate(random),
            });
        }
        const std::optional<PointSetDiameter3DResult> actual =
            point_set_diameter_3d(current);
        const std::optional<PointSetDiameter3DResult> bounded =
            point_set_diameter_3d_aabb_branch_and_bound(current);
        const PointSetDiameter3DResult expected = brute(current);
        if(!actual || actual->first != expected.first ||
           actual->second != expected.second ||
           std::abs(actual->distance - expected.distance) > 1e-12L *
               std::max(1.0L, expected.distance)) return 1;
        if(!bounded || bounded->first != expected.first ||
           bounded->second != expected.second ||
           bounded->distance != expected.distance) return 1;
    }

    std::vector<Point3> clustered;
    clustered.reserve(4002);
    clustered.push_back({-1000000, 0, 0});
    clustered.push_back({1000000, 0, 0});
    for(std::size_t index = 0; index < 4000; ++index){
        clustered.push_back({
            static_cast<long double>(coordinate(random)),
            static_cast<long double>(coordinate(random)),
            static_cast<long double>(coordinate(random)),
        });
    }
    const auto clustered_result =
        point_set_diameter_3d_aabb_branch_and_bound(clustered);
    if(!clustered_result || clustered_result->first != 0 ||
       clustered_result->second != 1 ||
       clustered_result->distance != 2000000) return 1;

    std::cout << "OK\n";
    return 0;
}
