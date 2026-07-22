// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/matching/bipartite/bipartite_vertex_cover.hpp"

int brute_minimum_vertex_cover(
    int left_size,
    int right_size,
    const std::vector<std::pair<int, int>>& edges
){
    const int vertex_count = left_size + right_size;
    int answer = vertex_count;
    for(int mask = 0; mask < (1 << vertex_count); ++mask){
        bool valid = true;
        for(const auto& [left, right]: edges){
            if(!(mask >> left & 1) && !(mask >> (left_size + right) & 1)){
                valid = false;
                break;
            }
        }
        if(valid) answer = std::min(answer, __builtin_popcount(static_cast<unsigned>(mask)));
    }
    return answer;
}

int brute_minimum_edge_cover(
    int left_size,
    int right_size,
    const std::vector<std::pair<int, int>>& edges
){
    const int vertex_count = left_size + right_size;
    if(vertex_count == 0) return 0;
    int answer = std::numeric_limits<int>::max();
    for(unsigned mask = 0; mask < (1U << edges.size()); ++mask){
        std::vector<char> covered(static_cast<std::size_t>(vertex_count));
        int used = 0;
        for(std::size_t index = 0; index < edges.size(); ++index){
            if(!(mask >> index & 1U)) continue;
            ++used;
            covered[static_cast<std::size_t>(edges[index].first)] = 1;
            covered[static_cast<std::size_t>(left_size + edges[index].second)] = 1;
        }
        if(std::all_of(covered.begin(), covered.end(), [](char value){ return value; })){
            answer = std::min(answer, used);
        }
    }
    return answer == std::numeric_limits<int>::max() ? -1 : answer;
}

void verify_case(
    int left_size,
    int right_size,
    const std::vector<std::pair<int, int>>& edges,
    bool brute_edges
){
    BipartiteMatching graph(left_size, right_size);
    for(const auto& [left, right]: edges) graph.add_edge(left, right);
    const auto matching = graph.solve();
    const auto decomposition = bipartite_konig_decomposition(graph, matching);

    assert(decomposition.minimum_vertex_cover.size == matching.size);
    assert(decomposition.maximum_independent_set.size
        == left_size + right_size - matching.size);
    for(const auto& [left, right]: edges){
        assert(decomposition.minimum_vertex_cover.left[static_cast<std::size_t>(left)]
            || decomposition.minimum_vertex_cover.right[static_cast<std::size_t>(right)]);
        assert(!(decomposition.maximum_independent_set.left[static_cast<std::size_t>(left)]
            && decomposition.maximum_independent_set.right[static_cast<std::size_t>(right)]));
    }
    assert(decomposition.minimum_vertex_cover.size
        == brute_minimum_vertex_cover(left_size, right_size, edges));

    const auto edge_cover = minimum_bipartite_edge_cover(graph, matching);
    if(!edge_cover){
        if(brute_edges){
            assert(brute_minimum_edge_cover(left_size, right_size, edges) == -1);
        }
        return;
    }
    std::vector<char> covered_left(static_cast<std::size_t>(left_size));
    std::vector<char> covered_right(static_cast<std::size_t>(right_size));
    for(const auto& [left, right]: *edge_cover){
        assert(std::find(
            graph.graph[static_cast<std::size_t>(left)].begin(),
            graph.graph[static_cast<std::size_t>(left)].end(),
            right
        ) != graph.graph[static_cast<std::size_t>(left)].end());
        covered_left[static_cast<std::size_t>(left)] = 1;
        covered_right[static_cast<std::size_t>(right)] = 1;
    }
    assert(std::all_of(covered_left.begin(), covered_left.end(), [](char value){ return value; }));
    assert(std::all_of(covered_right.begin(), covered_right.end(), [](char value){ return value; }));
    assert(edge_cover->size() == static_cast<std::size_t>(
        left_size + right_size - matching.size
    ));
    if(brute_edges){
        assert(static_cast<int>(edge_cover->size())
            == brute_minimum_edge_cover(left_size, right_size, edges));
    }
}

void self_test(){
    verify_case(0, 0, {}, true);
    verify_case(1, 1, {}, true);
    verify_case(2, 3, {{0, 0}, {0, 1}, {1, 1}, {1, 2}}, true);

    std::mt19937 random(2026071310);
    for(int left_size = 0; left_size <= 5; ++left_size){
        for(int right_size = 0; right_size <= 5; ++right_size){
            if(left_size + right_size == 0) continue;
            for(int iteration = 0; iteration < 300; ++iteration){
                std::vector<std::pair<int, int>> edges;
                for(int left = 0; left < left_size; ++left){
                    for(int right = 0; right < right_size; ++right){
                        if(random() % 3 == 0) edges.push_back({left, right});
                    }
                }
                verify_case(
                    left_size,
                    right_size,
                    edges,
                    edges.size() <= 18
                );
            }
        }
    }

    {
        BipartiteMatching graph(1, 1);
        graph.add_edge(0, 0);
        const BipartiteMatchingResult not_maximum{0, {-1}, {-1}};
        bool thrown = false;
        try{
            (void)bipartite_konig_decomposition(graph, not_maximum);
        }catch(const std::invalid_argument&){
            thrown = true;
        }
        assert(thrown);
    }

    constexpr int size = 100000;
    BipartiteMatching large(size, size);
    for(int vertex = 0; vertex < size; ++vertex) large.add_edge(vertex, vertex);
    const auto matching = large.solve();
    const auto decomposition = bipartite_konig_decomposition(large, matching);
    const auto edge_cover = minimum_bipartite_edge_cover(large, matching);
    assert(matching.size == size);
    assert(decomposition.minimum_vertex_cover.size == size);
    assert(edge_cover && edge_cover->size() == static_cast<std::size_t>(size));
}

int main(){
    int left_size, right_size, edge_count;
    if(!(std::cin >> left_size >> right_size >> edge_count)){
        self_test();
        return 0;
    }
    BipartiteMatching graph(left_size, right_size);
    for(int index = 0; index < edge_count; ++index){
        int left, right;
        std::cin >> left >> right;
        graph.add_edge(left, right);
    }
    const auto matching = graph.solve();
    const auto decomposition = bipartite_konig_decomposition(graph, matching);
    const auto edge_cover = minimum_bipartite_edge_cover(graph, matching);
    std::cout << matching.size << '\n';
    std::cout << decomposition.minimum_vertex_cover.size << '\n';
    std::cout << decomposition.maximum_independent_set.size << '\n';
    std::cout << (edge_cover ? static_cast<int>(edge_cover->size()) : -1) << '\n';
}
