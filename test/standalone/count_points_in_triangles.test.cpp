// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/type/definition/count_points_in_triangles.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/weighted_points_in_triangles.hpp"

void test_weighted_sums(){
    const std::vector<std::pair<long long, long long>> vertices{
        {0, 0}, {10, 0}, {0, 10}, {20, 0}, {0, 20}
    };
    const auto limit = std::numeric_limits<std::int64_t>::max();
    // Partial sums exceed int64; cancellation makes every answer fit again.
    const WeightedPointsInTriangles<long long> counter(vertices, {
        {1, 1, limit}, {1, 1, limit}, {1, 1, -limit}, {1, 1, -limit},
        {2, 1, 7}, {5, 5, 13}, {0, 5, 100},
        {40, 1, limit}, {50, 1, limit},
    });
    assert(counter.vertex_count() == vertices.size());
    assert(counter.weighted_sum(0, 1, 2) == 7);  // Boundary mass is excluded.
    assert(counter.weighted_sum(2, 1, 0) == 7);  // Orientation is immaterial.
    assert(counter.weighted_sum(0, 3, 4) == 20);
    assert(counter.weighted_sum(0, 1, 3) == 0);  // Degenerate triangle.
    const WeightedPointsInTriangles<long long> negative(vertices, {
        {1, 1, 5}, {2, 2, -8}
    });
    assert(negative.weighted_sum(0, 1, 2) == -3);
    const WeightedPointsInTriangles<long long> empty(vertices, {});
    assert(empty.weighted_sum(0, 1, 2) == 0);
    const WeightedPointsInTriangles<long long> overflowing(vertices, {
        {1, 1, limit}, {2, 1, limit}
    });
    bool thrown = false;
    try{
        (void)overflowing.weighted_sum(0, 1, 2);
    }catch(const std::overflow_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_invalid_indices(){
    const std::vector<std::pair<long long, long long>> vertices{
        {0, 0}, {4, 0}, {0, 4}
    };
    const std::vector<std::pair<long long, long long>> points{{1, 1}};
    const CountPointsInTriangles counter(vertices, points);
    for(const auto [first, second, third]:
        std::vector<std::array<std::size_t, 3>>{
            {3, 1, 2}, {0, 3, 2}, {0, 1, 3}
        }){
        bool thrown = false;
        try{
            static_cast<void>(counter.count(first, second, third));
        }catch(const std::out_of_range&){
            thrown = true;
        }
        assert(thrown);
    }
}

int main(){
    test_weighted_sums();
    test_invalid_indices();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t vertex_count;
    std::cin >> vertex_count;
    std::vector<std::pair<long long, long long>> vertices(vertex_count);
    for(auto& [x, y]: vertices) std::cin >> x >> y;
    std::size_t point_count;
    std::cin >> point_count;
    std::vector<std::pair<long long, long long>> points(point_count);
    for(auto& [x, y]: points) std::cin >> x >> y;

    const CountPointsInTriangles counter(vertices, points);
    std::size_t query_count;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::size_t first;
        std::size_t second;
        std::size_t third;
        std::cin >> first >> second >> third;
        std::cout << counter.count(first, second, third) << '\n';
    }
}
