// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "../../src/algorithm/matching/bipartite_matching.hpp"

int brute(int left_size, int right_size, const std::vector<std::pair<int, int>>& edges){
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(left_size));
    for(auto [l, r]: edges) graph[static_cast<std::size_t>(l)].push_back(r);
    std::vector<char> used(static_cast<std::size_t>(right_size), 0);
    int best = 0;
    auto dfs = [&](auto&& self, int v, int count) -> void {
        if(v == left_size){
            best = std::max(best, count);
            return;
        }
        self(self, v + 1, count);
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(!used[static_cast<std::size_t>(to)]){
                used[static_cast<std::size_t>(to)] = 1;
                self(self, v + 1, count + 1);
                used[static_cast<std::size_t>(to)] = 0;
            }
        }
    };
    dfs(dfs, 0, 0);
    return best;
}

void verify_matching(const BipartiteMatching& graph, const BipartiteMatchingResult& result){
    assert(result.left_match.size() == static_cast<std::size_t>(graph.left_size));
    assert(result.right_match.size() == static_cast<std::size_t>(graph.right_size));
    int count = 0;
    for(int left = 0; left < graph.left_size; ++left){
        const int right = result.left_match[static_cast<std::size_t>(left)];
        if(right == -1) continue;
        assert(0 <= right && right < graph.right_size);
        assert(result.right_match[static_cast<std::size_t>(right)] == left);
        assert(std::find(
            graph.graph[static_cast<std::size_t>(left)].begin(),
            graph.graph[static_cast<std::size_t>(left)].end(),
            right
        ) != graph.graph[static_cast<std::size_t>(left)].end());
        ++count;
    }
    assert(count == result.size);
}

void self_test(){
    {
        BipartiteMatching graph(3, 3);
        graph.add_edge(0, 0);
        graph.add_edge(0, 1);
        graph.add_edge(1, 1);
        graph.add_edge(2, 1);
        graph.add_edge(2, 2);
        const auto result = graph.solve();
        assert(result.size == 3);
        verify_matching(graph, result);
        verify_matching(graph, graph.solve());
    }
    {
        bool thrown = false;
        try{
            [[maybe_unused]] BipartiteMatching invalid(-1, 0);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        BipartiteMatching graph(1, 1);
        thrown = false;
        try{
            graph.add_edge(1, 0);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    std::mt19937 rng(20260822);
    for(int l = 0; l <= 9; l++){
        for(int r = 0; r <= 9; r++){
            for(int step = 0; step < 80; step++){
                BipartiteMatching graph(l, r);
                std::vector<std::pair<int, int>> edges;
                for(int i = 0; i < l; i++){
                    for(int j = 0; j < r; j++){
                        if(rng() % 3 == 0){
                            graph.add_edge(i, j);
                            edges.push_back({i, j});
                        }
                    }
                }
                const auto result = graph.solve();
                verify_matching(graph, result);
                assert(result.size == brute(l, r, edges));
            }
        }
    }
}

int main(){
    int l, r, m;
    if(!(std::cin >> l >> r >> m)){
        self_test();
        return 0;
    }
    BipartiteMatching graph(l, r);
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        graph.add_edge(u, v);
    }
    std::cout << graph.solve().size << '\n';
}
