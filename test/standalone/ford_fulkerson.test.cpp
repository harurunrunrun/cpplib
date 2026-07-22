// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "../../src/algorithm/matching/flow/ford_fulkerson.hpp"

long long brute_min_cut(
    int n,
    int s,
    int t,
    const std::vector<std::tuple<int, int, long long>>& edges
){
    long long best = (1LL << 60);
    for(int mask = 0; mask < (1 << n); mask++){
        if(((mask >> s) & 1) == 0 || ((mask >> t) & 1) != 0) continue;
        long long cut = 0;
        for(auto [u, v, c]: edges){
            if(((mask >> u) & 1) && !((mask >> v) & 1)) cut += c;
        }
        best = std::min(best, cut);
    }
    return best;
}

void self_test(){
    {
        bool thrown = false;
        try{
            [[maybe_unused]] FordFulkerson<int> graph(-1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        FordFulkerson<long long> graph(4);
        graph.add_edge(0, 1, 3);
        graph.add_edge(0, 2, 2);
        graph.add_edge(1, 2, 1);
        graph.add_edge(1, 3, 2);
        graph.add_edge(2, 3, 4);
        assert(graph.max_flow(0, 3) == 5);
    }
    {
        FordFulkerson<long long> graph(4);
        graph.add_edge(0, 1, 3);
        graph.add_edge(0, 2, 2);
        graph.add_edge(1, 3, 3);
        graph.add_edge(2, 3, 2);
        assert(graph.max_flow(0, 3, 2) == 2);
        assert(graph.max_flow(0, 3, 10) == 3);
        assert(graph.max_flow(0, 3) == 0);
        assert(graph.level.size() == 4);
        assert(graph.iter.size() == 4);
    }
    {
        FordFulkerson<long long> graph(2);
        graph.add_edge(0, 0, 7);
        assert(graph.graph[0].size() == 2);
        assert(graph.graph[0][0].rev == 1);
        assert(graph.graph[0][1].rev == 0);
        graph.add_edge(0, 1, 4);
        assert(graph.max_flow(0, 1) == 4);
    }
    {
        FordFulkerson<long long> graph(1);
        assert(graph.max_flow(0, 0, 123) == 0);
        bool thrown = false;
        try{
            (void)graph.max_flow(0, 0, -1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    std::mt19937 rng(20260826);
    for(int n = 2; n <= 10; n++){
        for(int step = 0; step < 80; step++){
            int s = 0, t = n - 1;
            FordFulkerson<long long> graph(n);
            std::vector<std::tuple<int, int, long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u != v && rng() % 5 == 0){
                        long long c = static_cast<int>(rng() % 6);
                        graph.add_edge(u, v, c);
                        edges.push_back({u, v, c});
                    }
                }
            }
            assert(graph.max_flow(s, t) == brute_min_cut(n, s, t, edges));
        }
    }
}

int main(){
    int n, m, s, t;
    if(!(std::cin >> n >> m >> s >> t)){
        self_test();
        return 0;
    }
    FordFulkerson<long long> graph(n);
    for(int i = 0; i < m; i++){
        int u, v;
        long long c;
        std::cin >> u >> v >> c;
        graph.add_edge(u, v, c);
    }
    std::cout << graph.max_flow(s, t) << '\n';
}
