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

#include "../../src/algorithm/geometry/3d/kd_tree_3d.hpp"

namespace{
long double point_distance(const Point3& left, const Point3& right){
    return std::hypot(left.x - right.x, left.y - right.y, left.z - right.z);
}

std::vector<std::pair<std::size_t, long double>> brute(
    const std::vector<Point3>& points,
    const Point3& query
){
    std::vector<std::pair<std::size_t, long double>> answer;
    for(std::size_t index = 0; index < points.size(); ++index){
        answer.push_back({index, point_distance(points[index], query)});
    }
    std::sort(answer.begin(), answer.end(), [](const auto& left, const auto& right){
        return left.second != right.second ? left.second < right.second
            : left.first < right.first;
    });
    return answer;
}

bool same(const auto& left, const auto& right){
    if(left.size() != right.size()) return false;
    for(std::size_t i = 0; i < left.size(); ++i){
        if(left[i].first != right[i].first || left[i].second != right[i].second) return false;
    }
    return true;
}
} // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    KdTree3D tree;
    if(!tree.empty() || tree.nearest({0, 0, 0}) || !tree.k_nearest({0, 0, 0}, 3).empty()) return 1;
    tree.build({{-1, 0, 0}, {1, 0, 0}, {0, 2, 0}, {-1, 0, 0}});
    const auto tied = tree.nearest({0, 0, 0});
    if(!tied || tied->first != 0 || tied->second != 1) return 1;
    if(tree.k_nearest({0, 0, 0}, 99).size() != 4
       || tree.radius_query({0, 0, 0}, 1).size() != 3) return 1;
    bool bad_radius = false;
    try{ static_cast<void>(tree.radius_query({0, 0, 0}, -1)); }
    catch(const std::invalid_argument&){ bad_radius = true; }
    if(!bad_radius) return 1;
    bool bad_point = false;
    try{ tree.build({{std::numeric_limits<long double>::quiet_NaN(), 0, 0}}); }
    catch(const std::invalid_argument&){ bad_point = true; }
    if(!bad_point) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-1000, 1000);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        const std::size_t size = static_cast<std::size_t>(random() % 90);
        std::vector<Point3> points;
        for(std::size_t i = 0; i < size; ++i){
            points.push_back({
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
            });
            if(i && random() % 17 == 0) points.back() = points[random() % i];
        }
        tree.build(points);
        const Point3 query{
            static_cast<long double>(coordinate(random)),
            static_cast<long double>(coordinate(random)),
            static_cast<long double>(coordinate(random)),
        };
        const auto expected = brute(points, query);
        const auto actual_nearest = tree.nearest(query);
        if(expected.empty()){
            if(actual_nearest) return 1;
        }else if(!actual_nearest || *actual_nearest != expected.front()) return 1;

        const std::size_t count = static_cast<std::size_t>(random() % 100);
        std::vector<std::pair<std::size_t, long double>> expected_k(
            expected.begin(), expected.begin() + static_cast<std::ptrdiff_t>(std::min(count, size))
        );
        if(!same(tree.k_nearest(query, count), expected_k)) return 1;

        const long double radius = static_cast<long double>(random() % 1500);
        std::vector<std::pair<std::size_t, long double>> expected_radius;
        for(const auto& item: expected) if(item.second <= radius) expected_radius.push_back(item);
        if(!same(tree.radius_query(query, radius), expected_radius)) return 1;
    }
    std::cout << "OK\n";
    return 0;
}
