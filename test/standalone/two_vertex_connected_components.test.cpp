// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/two_vertex_connected_components.hpp"

void validate(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    const TwoVertexConnectedComponentsResult& result
){
    std::vector<int> edge_count(edges.size(), 0);
    for(const auto& group: result.edge_groups){
        for(int id: group) edge_count[static_cast<std::size_t>(id)]++;
    }
    for(int count: edge_count) assert(count == 1);
    auto low = lowlink(n, edges);
    assert(result.articulation == low.articulation);
    assert(result.count == static_cast<int>(result.vertex_groups.size()));
    assert(result.count == static_cast<int>(result.edge_groups.size()));
}

void self_test(){
    {
        std::vector<std::pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 0}, {1, 3}, {3, 4}, {4, 1}};
        auto res = two_vertex_connected_components(5, edges);
        validate(5, edges, res);
        assert(res.count == 2);
        assert((res.articulation == std::vector<int>{1}));
    }
    {
        std::vector<std::pair<int, int>> edges = {{0, 1}, {0, 1}};
        auto res = two_vertex_connected_components(3, edges);
        validate(3, edges, res);
        assert(res.count == 2);
    }
    std::mt19937 rng(20260819);
    for(int n = 0; n <= 30; n++){
        for(int step = 0; step < 80; step++){
            std::vector<std::pair<int, int>> edges;
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 4 == 0) edges.push_back({u, v});
                    if(rng() % 23 == 0) edges.push_back({u, v});
                }
            }
            auto res = two_vertex_connected_components(n, edges);
            validate(n, edges, res);
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        edges.push_back({u, v});
    }
    auto res = two_vertex_connected_components(n, edges);
    std::cout << res.count << '\n';
}
