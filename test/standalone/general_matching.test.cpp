// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/matching/general_matching.hpp"

int brute(int n, const std::vector<std::pair<int, int>>& edges){
    std::vector<char> used(static_cast<std::size_t>(n), 0);
    int best = 0;
    auto dfs = [&](auto&& self, int id, int count) -> void {
        if(id == static_cast<int>(edges.size())){
            best = std::max(best, count);
            return;
        }
        self(self, id + 1, count);
        auto [u, v] = edges[static_cast<std::size_t>(id)];
        if(!used[static_cast<std::size_t>(u)] && !used[static_cast<std::size_t>(v)]){
            used[static_cast<std::size_t>(u)] = used[static_cast<std::size_t>(v)] = 1;
            self(self, id + 1, count + 1);
            used[static_cast<std::size_t>(u)] = used[static_cast<std::size_t>(v)] = 0;
        }
    };
    dfs(dfs, 0, 0);
    return best;
}

bool valid(int n, const GeneralMatchingResult& result){
    int count = 0;
    for(int i = 0; i < n; i++){
        int j = result.match[static_cast<std::size_t>(i)];
        if(j == -1) continue;
        if(j < 0 || n <= j) return false;
        if(result.match[static_cast<std::size_t>(j)] != i) return false;
        if(i < j) count++;
    }
    return count == result.size;
}

void self_test(){
    {
        GeneralMatching graph(5);
        graph.add_edge(0, 1);
        graph.add_edge(1, 2);
        graph.add_edge(2, 0);
        graph.add_edge(3, 4);
        auto res = graph.solve();
        assert(res.size == 2);
        assert(valid(5, res));
    }
    std::mt19937 rng(20260828);
    for(int n = 0; n <= 12; n++){
        for(int step = 0; step < 80; step++){
            GeneralMatching graph(n);
            std::vector<std::pair<int, int>> edges;
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 4 == 0){
                        graph.add_edge(u, v);
                        edges.push_back({u, v});
                    }
                }
            }
            auto res = graph.solve();
            assert(valid(n, res));
            assert(res.size == brute(n, edges));
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    GeneralMatching graph(n);
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        graph.add_edge(u, v);
    }
    std::cout << graph.solve().size << '\n';
}
