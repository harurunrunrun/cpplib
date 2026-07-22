// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/connectivity/dominator_tree.hpp"

std::vector<int> naive_immediate_dominator(
    const std::vector<std::vector<int>>& graph,
    int source
){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<int>> predecessor(static_cast<std::size_t>(n));
    std::vector<unsigned char> reachable(static_cast<std::size_t>(n), 0);
    std::queue<int> queue;
    reachable[static_cast<std::size_t>(source)] = 1;
    queue.push(source);
    while(!queue.empty()){
        const int vertex = queue.front();
        queue.pop();
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            predecessor[static_cast<std::size_t>(to)].push_back(vertex);
            if(!reachable[static_cast<std::size_t>(to)]){
                reachable[static_cast<std::size_t>(to)] = 1;
                queue.push(to);
            }
        }
    }
    std::uint64_t all = 0;
    for(int vertex = 0; vertex < n; ++vertex){
        if(reachable[static_cast<std::size_t>(vertex)]) all |= std::uint64_t{1} << vertex;
    }
    std::vector<std::uint64_t> dominators(static_cast<std::size_t>(n), 0);
    for(int vertex = 0; vertex < n; ++vertex){
        if(reachable[static_cast<std::size_t>(vertex)]) dominators[static_cast<std::size_t>(vertex)] = all;
    }
    dominators[static_cast<std::size_t>(source)] = std::uint64_t{1} << source;
    bool changed = true;
    while(changed){
        changed = false;
        for(int vertex = 0; vertex < n; ++vertex){
            if(vertex == source || !reachable[static_cast<std::size_t>(vertex)]) continue;
            std::uint64_t intersection = all;
            for(int from: predecessor[static_cast<std::size_t>(vertex)]){
                if(reachable[static_cast<std::size_t>(from)]){
                    intersection &= dominators[static_cast<std::size_t>(from)];
                }
            }
            const std::uint64_t next = intersection | (std::uint64_t{1} << vertex);
            if(next != dominators[static_cast<std::size_t>(vertex)]){
                dominators[static_cast<std::size_t>(vertex)] = next;
                changed = true;
            }
        }
    }
    std::vector<int> answer(static_cast<std::size_t>(n), -1);
    answer[static_cast<std::size_t>(source)] = source;
    for(int vertex = 0; vertex < n; ++vertex){
        if(vertex == source || !reachable[static_cast<std::size_t>(vertex)]) continue;
        const std::uint64_t strict = dominators[static_cast<std::size_t>(vertex)] &
            ~(std::uint64_t{1} << vertex);
        int best = -1;
        int best_depth = -1;
        for(int candidate = 0; candidate < n; ++candidate){
            if((strict >> candidate) & 1U){
                const int depth = __builtin_popcountll(
                    dominators[static_cast<std::size_t>(candidate)]
                );
                if(best_depth < depth){
                    best_depth = depth;
                    best = candidate;
                }
            }
        }
        answer[static_cast<std::size_t>(vertex)] = best;
    }
    return answer;
}

void test_random(){
    std::mt19937 rng(2026071453);
    for(int iteration = 0; iteration < 1500; ++iteration){
        const int n = 1 + static_cast<int>(rng() % 18);
        const int source = static_cast<int>(rng() % n);
        std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
        for(int from = 0; from < n; ++from){
            for(int to = 0; to < n; ++to){
                if(rng() % 100 < 18) graph[static_cast<std::size_t>(from)].push_back(to);
            }
        }
        const auto result = dominator_tree(graph, source);
        assert(result.immediate_dominator == naive_immediate_dominator(graph, source));
        for(int vertex = 0; vertex < n; ++vertex){
            const int parent = result.immediate_dominator[static_cast<std::size_t>(vertex)];
            if(parent != -1 && vertex != source){
                assert(std::find(result.tree[static_cast<std::size_t>(parent)].begin(),
                                 result.tree[static_cast<std::size_t>(parent)].end(), vertex) !=
                       result.tree[static_cast<std::size_t>(parent)].end());
            }
        }
    }
}

void test_deep_and_errors(){
    constexpr int n = 60000;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int vertex = 0; vertex + 1 < n; ++vertex){
        graph[static_cast<std::size_t>(vertex)].push_back(vertex + 1);
    }
    const auto result = dominator_tree(graph, 0);
    assert(result.immediate_dominator[0] == 0);
    assert(result.immediate_dominator[1] == 0);
    for(int vertex = 2; vertex < n; ++vertex){
        assert(result.immediate_dominator[static_cast<std::size_t>(vertex)] == vertex - 1);
    }

    bool thrown = false;
    try{ (void)dominator_tree(std::vector<std::vector<int>>(2), 2); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)dominator_tree(std::vector<std::vector<int>>{{2}, {}}, 0); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, edge_count, source;
    if(!(std::cin >> n >> edge_count >> source)){
        test_random();
        test_deep_and_errors();
        return 0;
    }
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    while(edge_count-- > 0){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    const auto result = dominator_tree(graph, source);
    for(int vertex = 0; vertex < n; ++vertex){
        if(vertex != 0) std::cout << ' ';
        std::cout << result.immediate_dominator[static_cast<std::size_t>(vertex)];
    }
    std::cout << '\n';
}
