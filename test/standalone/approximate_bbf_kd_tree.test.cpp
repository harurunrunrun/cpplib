// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/nearest_neighbor/best_bin_first_kd_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t n;
    std::size_t query_count;
    if(!(std::cin >> n >> query_count)) return 0;
    std::vector<std::array<long long, 3>> points(n);
    for(auto& point: points) std::cin >> point[0] >> point[1] >> point[2];
    approximate::nearest_neighbor::BestBinFirstKdTree<long long, 3> tree(points);
    assert(tree.size() == points.size());
    assert(tree.empty() == points.empty());
    for(std::size_t query_index = 0; query_index < query_count; ++query_index){
        std::array<long long, 3> query{};
        std::cin >> query[0] >> query[1] >> query[2];
        const auto result = tree.nearest(query);
        if(points.empty()){
            assert(!result.has_value());
            std::cout << "none\n";
            continue;
        }
        std::size_t expected_index = 0;
        long double expected_distance = std::numeric_limits<long double>::infinity();
        for(std::size_t index = 0; index < points.size(); ++index){
            long double distance = 0.0L;
            for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
                const long double difference = static_cast<long double>(query[coordinate])
                    - static_cast<long double>(points[index][coordinate]);
                distance += difference * difference;
            }
            if(distance < expected_distance){
                expected_distance = distance;
                expected_index = index;
            }
        }
        assert(result.has_value());
        assert(result->index == expected_index);
        assert(result->squared_distance == expected_distance);
        const auto approximate = tree.nearest(query, std::min<std::size_t>(3, points.size()));
        assert(approximate.has_value());
        assert(approximate->checked_points <= std::min<std::size_t>(3, points.size()));
        std::cout << result->index << ' ' << std::llround(result->squared_distance) << '\n';
    }
    bool invalid = false;
    try{
        static_cast<void>(tree.nearest(std::array<double, 3>{0.0, 0.0, 0.0}, 0));
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{
        approximate::nearest_neighbor::BestBinFirstKdTree<double, 2> bad(
            {{{0.0, std::numeric_limits<double>::infinity()}}}
        );
        static_cast<void>(bad);
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
}
