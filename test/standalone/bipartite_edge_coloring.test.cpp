// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/bipartite_edge_coloring.hpp"

int max_degree(int l, int r, const std::vector<BipartiteEdgeColoringEdge>& edges){
    std::vector<int> dl(static_cast<std::size_t>(l), 0), dr(static_cast<std::size_t>(r), 0);
    for(auto e: edges){
        dl[static_cast<std::size_t>(e.left)]++;
        dr[static_cast<std::size_t>(e.right)]++;
    }
    int result = 0;
    for(int x: dl) result = std::max(result, x);
    for(int x: dr) result = std::max(result, x);
    return result;
}

bool valid_coloring(
    int l,
    int r,
    const std::vector<BipartiteEdgeColoringEdge>& edges,
    const BipartiteEdgeColoringResult& coloring
){
    int delta = max_degree(l, r, edges);
    if(coloring.color_count != delta) return false;
    if(coloring.color.size() != edges.size()) return false;
    std::vector<std::vector<char>> used_l(static_cast<std::size_t>(l), std::vector<char>(static_cast<std::size_t>(delta), 0));
    std::vector<std::vector<char>> used_r(static_cast<std::size_t>(r), std::vector<char>(static_cast<std::size_t>(delta), 0));
    for(std::size_t i = 0; i < edges.size(); i++){
        int c = coloring.color[i];
        if(c < 0 || delta <= c) return false;
        auto e = edges[i];
        if(used_l[static_cast<std::size_t>(e.left)][static_cast<std::size_t>(c)]) return false;
        if(used_r[static_cast<std::size_t>(e.right)][static_cast<std::size_t>(c)]) return false;
        used_l[static_cast<std::size_t>(e.left)][static_cast<std::size_t>(c)] = 1;
        used_r[static_cast<std::size_t>(e.right)][static_cast<std::size_t>(c)] = 1;
    }
    return true;
}

void self_test(){
    {
        std::vector<BipartiteEdgeColoringEdge> edges = {
            {0, 0}, {0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 1}
        };
        auto res = bipartite_edge_coloring(3, 2, edges);
        assert(valid_coloring(3, 2, edges, res));
    }
    std::mt19937 rng(20260803);
    for(int l = 1; l <= 9; l++){
        for(int r = 1; r <= 9; r++){
            for(int step = 0; step < 50; step++){
                std::vector<BipartiteEdgeColoringEdge> edges;
                for(int u = 0; u < l; u++){
                    for(int v = 0; v < r; v++){
                        int cnt = static_cast<int>(rng() % 3);
                        while(cnt--) edges.push_back({u, v});
                    }
                }
                auto res = bipartite_edge_coloring(l, r, edges);
                assert(valid_coloring(l, r, edges, res));
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
    std::vector<BipartiteEdgeColoringEdge> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        edges.push_back({u, v});
    }
    auto res = bipartite_edge_coloring(l, r, edges);
    std::cout << res.color_count << ' ' << static_cast<int>(valid_coloring(l, r, edges, res)) << '\n';
}
