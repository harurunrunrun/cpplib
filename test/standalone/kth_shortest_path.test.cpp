// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "../../src/algorithm/graph/shortest_path/kth_shortest_path.hpp"

struct BrutePath{
    long long cost;
    std::vector<int> vertices;
};

std::vector<BrutePath> brute_paths(
    const std::vector<std::vector<KthShortestPathEdge<long long>>>& graph,
    int s,
    int t,
    int k
){
    const int n = static_cast<int>(graph.size());
    std::vector<BrutePath> all;
    std::vector<char> used(static_cast<std::size_t>(n), 0);
    std::vector<int> path;
    auto dfs = [&](auto&& self, int v, long long cost) -> void {
        used[static_cast<std::size_t>(v)] = 1;
        path.push_back(v);
        if(v == t){
            all.push_back({cost, path});
        }else{
            for(auto e: graph[static_cast<std::size_t>(v)]){
                if(!used[static_cast<std::size_t>(e.to)]){
                    self(self, e.to, cost + e.cost);
                }
            }
        }
        path.pop_back();
        used[static_cast<std::size_t>(v)] = 0;
    };
    dfs(dfs, s, 0);
    std::sort(all.begin(), all.end(), [](const BrutePath& a, const BrutePath& b){
        if(a.cost != b.cost) return a.cost < b.cost;
        return a.vertices < b.vertices;
    });
    all.erase(std::unique(all.begin(), all.end(), [](const BrutePath& a, const BrutePath& b){
        return a.vertices == b.vertices;
    }), all.end());
    if(static_cast<int>(all.size()) > k) all.resize(static_cast<std::size_t>(k));
    return all;
}

void self_test(){
    {
        std::vector<std::vector<KthShortestPathEdge<long long>>> graph(4);
        graph[0].push_back({1, 1});
        graph[0].push_back({2, 2});
        graph[1].push_back({3, 3});
        graph[2].push_back({3, 1});
        graph[1].push_back({2, 1});
        auto paths = kth_shortest_paths<long long>(graph, 0, 3, 3);
        assert(paths.size() == 3);
        assert(paths[0].cost == 3);
        assert((paths[0].vertices == std::vector<int>{0, 2, 3}));
        assert(paths[1].cost == 3);
        assert((paths[1].vertices == std::vector<int>{0, 1, 2, 3}));
        assert(paths[2].cost == 4);
    }
    {
        constexpr long long INF = 1LL << 60;
        std::vector<std::vector<KthShortestPathEdge<long long>>> graph(3);
        graph[0].push_back({1, INF});
        graph[1].push_back({2, 2000000000000000000LL});
        graph[0].push_back({2, 3000000000000000000LL});
        auto paths = kth_shortest_paths<long long>(graph, 0, 2, 2, INF);
        assert(paths.size() == 2);
        assert(paths[0].cost == 3000000000000000000LL);
        assert(paths[1].cost == INF + 2000000000000000000LL);
    }
    {
        std::vector<std::vector<KthShortestPathEdge<long long>>> graph(2);
        graph[1].push_back({1, -1});
        bool thrown = false;
        try{
            (void)kth_shortest_paths<long long>(graph, 0, 0, 1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }

    std::mt19937 rng(20260729);
    for(int n = 1; n <= 8; n++){
        for(int step = 0; step < 200; step++){
            std::vector<std::vector<KthShortestPathEdge<long long>>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u != v && rng() % 4 == 0){
                        graph[static_cast<std::size_t>(u)].push_back({v, static_cast<int>(rng() % 8)});
                    }
                }
            }
            int s = static_cast<int>(rng() % n);
            int t = static_cast<int>(rng() % n);
            int k = 6;
            auto expected = brute_paths(graph, s, t, k);
            auto actual = kth_shortest_paths<long long>(graph, s, t, k);
            assert(actual.size() == expected.size());
            for(std::size_t i = 0; i < actual.size(); i++){
                assert(actual[i].cost == expected[i].cost);
            }
        }
    }
}

int main(){
    int n, m, s, t, k;
    if(!(std::cin >> n >> m >> s >> t >> k)){
        self_test();
        return 0;
    }
    std::vector<std::vector<KthShortestPathEdge<long long>>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        graph[static_cast<std::size_t>(u)].push_back({v, w});
    }
    auto paths = kth_shortest_paths<long long>(graph, s, t, k, 1LL << 60);
    std::cout << paths.size() << '\n';
    for(std::size_t i = 0; i < paths.size(); i++){
        if(i) std::cout << ' ';
        std::cout << paths[i].cost;
    }
    std::cout << '\n';
}
