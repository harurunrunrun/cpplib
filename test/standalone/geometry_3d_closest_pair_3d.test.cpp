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

#include "../../src/algorithm/geometry/3d/point_collection/closest_pair_3d.hpp"

namespace{

ClosestPair3DResult brute(const std::vector<Point3>& points){
    ClosestPair3DResult result{
        0, 1, std::hypot(
            points[0].x - points[1].x,
            points[0].y - points[1].y,
            points[0].z - points[1].z
        ),
    };
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1; second < points.size(); ++second){
            const long double distance = std::hypot(
                points[first].x - points[second].x,
                points[first].y - points[second].y,
                points[first].z - points[second].z
            );
            if(distance < result.distance ||
               (distance == result.distance &&
                std::pair{first, second} < std::pair{result.first, result.second})){
                result = {first, second, distance};
            }
        }
    }
    return result;
}

bool same(const ClosestPair3DResult& left, const ClosestPair3DResult& right){
    return left.first == right.first && left.second == right.second &&
        std::abs(left.distance - right.distance) <= 2e-12L *
            std::max({1.0L, left.distance, right.distance});
}

} // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds, performance_size = 0;
    if(!(std::cin >> seed >> rounds) || rounds > 10000) return 2;
    if(std::cin >> performance_size){
        if(performance_size > 300000) return 2;
    }else{
        std::cin.clear();
    }
    if(rounds == 0 && performance_size == 0) return 2;
    if(closest_pair_3d({}) || closest_pair_3d({{1, 2, 3}})) return 1;

    const std::vector<Point3> duplicate{{
        {9, 9, 9}, {1, 2, 3}, {4, 5, 6}, {1, 2, 3}, {1, 2, 3},
    }};
    const auto duplicate_result = closest_pair_3d(duplicate);
    if(!duplicate_result || duplicate_result->first != 1 ||
       duplicate_result->second != 3 || duplicate_result->distance != 0) return 1;

    const long double shift = 1e15L;
    const std::vector<Point3> translated{{
        {shift, -shift, shift}, {shift + 100, -shift, shift},
        {shift + 3, -shift + 4, shift}, {shift - 90, -shift, shift},
    }};
    const auto translated_result = closest_pair_3d(translated);
    if(!translated_result || translated_result->first != 0 ||
       translated_result->second != 2 ||
       std::abs(translated_result->distance - 5) > 1e-12L) return 1;

    bool nonfinite_threw = false;
    try{
        static_cast<void>(closest_pair_3d({
            {0, 0, 0}, {std::numeric_limits<long double>::infinity(), 0, 0},
        }));
    }catch(const std::invalid_argument&){
        nonfinite_threw = true;
    }
    if(!nonfinite_threw) return 1;

    const auto tied = closest_pair_3d({
        {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
    });
    if(!tied || tied->first != 0 || tied->second != 1 ||
       tied->distance != 1) return 1;

    const long double huge = std::ldexp(
        0.75L, std::numeric_limits<long double>::max_exponent - 1
    );
    const std::vector<Point3> extreme{{
        {huge, huge, huge}, {-huge, -huge, -huge},
        {0, 0, 0}, {1, 0, 0}, {10, 0, 0},
    }};
    const auto extreme_result = closest_pair_3d(extreme);
    if(!extreme_result || extreme_result->first != 2 ||
       extreme_result->second != 3 || extreme_result->distance != 1) return 1;
    const auto overflowing_distance = closest_pair_3d({
        {huge, huge, huge}, {-huge, -huge, -huge},
    });
    if(!overflowing_distance || overflowing_distance->first != 0 ||
       overflowing_distance->second != 1 ||
       !std::isinf(overflowing_distance->distance)) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-1000, 1000);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        const std::size_t size = 2 + static_cast<std::size_t>(random() % 70);
        std::vector<Point3> points;
        points.reserve(size);
        for(std::size_t index = 0; index < size; ++index){
            points.push_back({
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
            });
            if(index > 0 && random() % 31 == 0) points.back() = points[index - 1];
        }
        const std::optional<ClosestPair3DResult> actual = closest_pair_3d(points);
        if(!actual || !same(*actual, brute(points))) return 1;
    }

    if(performance_size != 0){
        std::vector<Point3> points;
        points.reserve(performance_size);
        for(std::size_t index = 0; index < performance_size; ++index){
            const long double value = static_cast<long double>(index);
            points.push_back({3 * value, 4 * value, 12 * value});
        }
        const auto result = closest_pair_3d(points);
        if(!result || result->first != 0 || result->second != 1 ||
           result->distance != 13) return 1;
    }
    std::cout << "OK\n";
    return 0;
}
