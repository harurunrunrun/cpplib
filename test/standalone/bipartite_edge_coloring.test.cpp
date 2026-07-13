// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/bipartite_edge_coloring.hpp"

int maximum_degree(
    int left_size,
    int right_size,
    const std::vector<BipartiteEdgeColoringEdge>& edges
){
    std::vector<int> left_degree(static_cast<std::size_t>(left_size));
    std::vector<int> right_degree(static_cast<std::size_t>(right_size));
    int result = 0;
    for(const auto& edge: edges){
        result = std::max(
            result, ++left_degree[static_cast<std::size_t>(edge.left)]
        );
        result = std::max(
            result, ++right_degree[static_cast<std::size_t>(edge.right)]
        );
    }
    return result;
}

bool valid_coloring(
    int left_size,
    int right_size,
    const std::vector<BipartiteEdgeColoringEdge>& edges,
    const BipartiteEdgeColoringResult& coloring
){
    const int delta = maximum_degree(left_size, right_size, edges);
    if(coloring.color_count != delta || coloring.color.size() != edges.size()){
        return false;
    }
    std::vector<std::uint64_t> left_keys;
    std::vector<std::uint64_t> right_keys;
    left_keys.reserve(edges.size());
    right_keys.reserve(edges.size());
    for(std::size_t edge_id = 0; edge_id < edges.size(); ++edge_id){
        const int color = coloring.color[edge_id];
        if(color < 0 || delta <= color) return false;
        const auto& edge = edges[edge_id];
        left_keys.push_back(
            static_cast<std::uint64_t>(
                static_cast<unsigned int>(edge.left)
            ) << 32U |
            static_cast<unsigned int>(color)
        );
        right_keys.push_back(
            static_cast<std::uint64_t>(
                static_cast<unsigned int>(edge.right)
            ) << 32U |
            static_cast<unsigned int>(color)
        );
    }
    std::sort(left_keys.begin(), left_keys.end());
    std::sort(right_keys.begin(), right_keys.end());
    return std::adjacent_find(left_keys.begin(), left_keys.end()) ==
               left_keys.end() &&
        std::adjacent_find(right_keys.begin(), right_keys.end()) ==
               right_keys.end();
}

bool brute_colorable(
    int left_size,
    int right_size,
    const std::vector<BipartiteEdgeColoringEdge>& edges,
    int color_count
){
    if(edges.empty()) return true;
    if(color_count <= 0 || color_count > 30) return false;
    std::vector<int> left_degree(static_cast<std::size_t>(left_size));
    std::vector<int> right_degree(static_cast<std::size_t>(right_size));
    for(const auto& edge: edges){
        ++left_degree[static_cast<std::size_t>(edge.left)];
        ++right_degree[static_cast<std::size_t>(edge.right)];
    }
    std::vector<int> order(edges.size());
    std::iota(order.begin(), order.end(), 0);
    std::stable_sort(order.begin(), order.end(), [&](int first, int second){
        const auto& left_edge = edges[static_cast<std::size_t>(first)];
        const auto& right_edge = edges[static_cast<std::size_t>(second)];
        return left_degree[static_cast<std::size_t>(left_edge.left)] +
                right_degree[static_cast<std::size_t>(left_edge.right)] >
            left_degree[static_cast<std::size_t>(right_edge.left)] +
                right_degree[static_cast<std::size_t>(right_edge.right)];
    });
    std::vector<unsigned int> used_left(static_cast<std::size_t>(left_size));
    std::vector<unsigned int> used_right(static_cast<std::size_t>(right_size));
    const std::function<bool(std::size_t)> search = [&](std::size_t index){
        if(index == order.size()) return true;
        const auto& edge = edges[static_cast<std::size_t>(order[index])];
        const unsigned int unavailable =
            used_left[static_cast<std::size_t>(edge.left)] |
            used_right[static_cast<std::size_t>(edge.right)];
        for(int color = 0; color < color_count; ++color){
            const unsigned int bit = 1U << color;
            if(unavailable & bit) continue;
            used_left[static_cast<std::size_t>(edge.left)] |= bit;
            used_right[static_cast<std::size_t>(edge.right)] |= bit;
            if(search(index + 1)) return true;
            used_left[static_cast<std::size_t>(edge.left)] ^= bit;
            used_right[static_cast<std::size_t>(edge.right)] ^= bit;
        }
        return false;
    };
    return search(0);
}

void check_case(
    int left_size,
    int right_size,
    const std::vector<BipartiteEdgeColoringEdge>& edges,
    bool compare_brute
){
    const auto result = bipartite_edge_coloring(
        left_size, right_size, edges
    );
    assert(valid_coloring(left_size, right_size, edges, result));
    if(compare_brute){
        const int delta = maximum_degree(left_size, right_size, edges);
        assert(brute_colorable(left_size, right_size, edges, delta));
        if(delta != 0){
            assert(!brute_colorable(left_size, right_size, edges, delta - 1));
        }
    }
}

void test_random(){
    std::mt19937 rng(2026071801);
    for(int iteration = 0; iteration < 1400; ++iteration){
        const int left_size = 1 + static_cast<int>(rng() % 6U);
        const int right_size = 1 + static_cast<int>(rng() % 6U);
        const int edge_count = static_cast<int>(rng() % 11U);
        std::vector<BipartiteEdgeColoringEdge> edges;
        for(int edge = 0; edge < edge_count; ++edge){
            edges.push_back({
                static_cast<int>(
                    rng() % static_cast<unsigned int>(left_size)
                ),
                static_cast<int>(
                    rng() % static_cast<unsigned int>(right_size)
                )
            });
        }
        check_case(left_size, right_size, edges, true);
    }
}

void test_boundaries(){
    check_case(0, 0, {}, true);
    check_case(100000, 100000, {}, true);
    check_case(3, 4, {{0, 1}}, true);
    check_case(
        3, 2,
        {{0, 0}, {0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 1}},
        true
    );

    constexpr int edge_count = 100000;
    std::vector<BipartiteEdgeColoringEdge> matching;
    matching.reserve(static_cast<std::size_t>(edge_count));
    for(int vertex = 0; vertex < edge_count; ++vertex){
        matching.push_back({vertex, vertex});
    }
    check_case(edge_count, edge_count, matching, false);

    std::vector<BipartiteEdgeColoringEdge> parallel(
        static_cast<std::size_t>(edge_count), {0, 0}
    );
    check_case(100000, 100000, parallel, false);

    constexpr int regular_size = 33333;
    std::vector<BipartiteEdgeColoringEdge> regular;
    regular.reserve(static_cast<std::size_t>(3 * regular_size));
    for(int left = 0; left < regular_size; ++left){
        regular.push_back({left, left});
        regular.push_back({left, (left + 1) % regular_size});
        regular.push_back({left, (left + 2) % regular_size});
    }
    check_case(regular_size, regular_size, regular, false);

    bool thrown = false;
    try{ (void)bipartite_edge_coloring(0, 1, {{0, 0}}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)bipartite_edge_coloring(-1, 0, {}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int left_size, right_size, edge_count;
    if(!(std::cin >> left_size >> right_size >> edge_count)){
        test_random();
        test_boundaries();
        return 0;
    }
    std::vector<BipartiteEdgeColoringEdge> edges(
        static_cast<std::size_t>(edge_count)
    );
    for(auto& edge: edges) std::cin >> edge.left >> edge.right;
    const auto result = bipartite_edge_coloring(
        left_size, right_size, edges
    );
    std::cout << result.color_count << ' '
        << static_cast<int>(
            valid_coloring(left_size, right_size, edges, result)
        ) << '\n';
}
