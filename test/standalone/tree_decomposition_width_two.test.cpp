// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/tree_decomposition_width_two.hpp"

void validate(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges,
    const TreeDecompositionWidthTwoResult& result
){
    assert(result.valid);
    const int bag_count = static_cast<int>(result.bags.size());
    if(vertex_count == 0){
        assert(bag_count == 0 && result.tree_edges.empty());
        return;
    }
    assert(bag_count == vertex_count);
    assert(result.tree_edges.size() + 1 == result.bags.size());
    std::vector<std::vector<int>> tree(static_cast<std::size_t>(bag_count));
    for(const auto [first, second]: result.tree_edges){
        assert(0 <= first && first < bag_count);
        assert(0 <= second && second < bag_count);
        tree[static_cast<std::size_t>(first)].push_back(second);
        tree[static_cast<std::size_t>(second)].push_back(first);
    }
    std::vector<unsigned char> seen(static_cast<std::size_t>(bag_count));
    std::queue<int> queue;
    queue.push(0);
    seen[0] = 1;
    while(!queue.empty()){
        const int current = queue.front();
        queue.pop();
        for(const int next: tree[static_cast<std::size_t>(current)]){
            if(!seen[static_cast<std::size_t>(next)]){
                seen[static_cast<std::size_t>(next)] = 1;
                queue.push(next);
            }
        }
    }
    assert(std::all_of(seen.begin(), seen.end(), [](unsigned char value){ return value; }));

    std::set<std::pair<int, int>> covered_edges;
    std::vector<int> occurrence_count(static_cast<std::size_t>(vertex_count));
    for(const auto& bag: result.bags){
        assert(!bag.empty() && bag.size() <= 3);
        for(const int vertex: bag){
            assert(0 <= vertex && vertex < vertex_count);
            ++occurrence_count[static_cast<std::size_t>(vertex)];
        }
        for(std::size_t first = 0; first < bag.size(); ++first){
            for(std::size_t second = first + 1; second < bag.size(); ++second){
                covered_edges.emplace(
                    std::min(bag[first], bag[second]),
                    std::max(bag[first], bag[second])
                );
            }
        }
    }
    for(const auto [first, second]: edges){
        assert(covered_edges.contains({
            std::min(first, second), std::max(first, second)
        }));
    }
    std::vector<int> induced_edge_count(static_cast<std::size_t>(vertex_count));
    for(const auto [first, second]: result.tree_edges){
        for(const int left_vertex: result.bags[static_cast<std::size_t>(first)]){
            for(const int right_vertex: result.bags[static_cast<std::size_t>(second)]){
                if(left_vertex == right_vertex){
                    ++induced_edge_count[static_cast<std::size_t>(left_vertex)];
                }
            }
        }
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        assert(occurrence_count[static_cast<std::size_t>(vertex)] > 0);
        assert(induced_edge_count[static_cast<std::size_t>(vertex)] + 1 ==
            occurrence_count[static_cast<std::size_t>(vertex)]);
    }
}

bool brute_width_two(std::vector<std::set<int>> graph, int remaining){
    if(remaining == 0) return true;
    for(std::size_t vertex = 0; vertex < graph.size(); ++vertex){
        if(graph[vertex].contains(static_cast<int>(vertex)) ||
           graph[vertex].size() > 2) continue;
        auto next = graph;
        std::vector<int> neighbors(next[vertex].begin(), next[vertex].end());
        for(const int neighbor: neighbors) next[static_cast<std::size_t>(neighbor)].erase(static_cast<int>(vertex));
        next[vertex].clear();
        if(neighbors.size() == 2){
            next[static_cast<std::size_t>(neighbors[0])].insert(neighbors[1]);
            next[static_cast<std::size_t>(neighbors[1])].insert(neighbors[0]);
        }
        next[vertex].insert(static_cast<int>(vertex));
        if(brute_width_two(std::move(next), remaining - 1)) return true;
    }
    return false;
}

void self_test(){
    validate(0, {}, tree_decomposition_width_two(0, {}));
    validate(1, {}, tree_decomposition_width_two(1, {}));
    validate(5, {{0,1},{1,2},{2,3},{3,4},{4,0}},
        tree_decomposition_width_two(5, {{0,1},{1,2},{2,3},{3,4},{4,0}}));
    const std::vector<std::pair<int,int>> k4{
        {0,1},{0,2},{0,3},{1,2},{1,3},{2,3}
    };
    assert(!tree_decomposition_width_two(4, k4).valid);

    std::mt19937_64 random(2026071617);
    for(int n = 1; n <= 8; ++n){
        for(int iteration = 0; iteration < 1000; ++iteration){
            std::vector<std::pair<int,int>> edges;
            std::vector<std::set<int>> graph(static_cast<std::size_t>(n));
            for(int first = 0; first < n; ++first){
                for(int second = first + 1; second < n; ++second){
                    if(random() % 3 == 0){
                        edges.emplace_back(first, second);
                        graph[static_cast<std::size_t>(first)].insert(second);
                        graph[static_cast<std::size_t>(second)].insert(first);
                    }
                }
            }
            const bool expected = brute_width_two(graph, n);
            const auto result = tree_decomposition_width_two(n, edges);
            assert(result.valid == expected);
            if(result.valid) validate(n, edges, result);
        }
    }

    constexpr int large_size = 100000;
    std::vector<std::pair<int,int>> cycle;
    cycle.reserve(large_size);
    for(int vertex = 0; vertex < large_size; ++vertex){
        cycle.emplace_back(vertex, (vertex + 1) % large_size);
    }
    const auto large = tree_decomposition_width_two(large_size, cycle);
    assert(large.valid && large.width() == 2);
}

int main(){
    int vertex_count;
    int edge_count;
    if(!(std::cin >> vertex_count >> edge_count)){
        self_test();
        return 0;
    }
    std::vector<std::pair<int,int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [first, second]: edges) std::cin >> first >> second;
    const auto result = tree_decomposition_width_two(vertex_count, edges);
    if(result.valid) validate(vertex_count, edges, result);
    std::cout << static_cast<int>(result.valid) << '\n';
}
