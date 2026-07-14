// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "../../src/algorithm/matching/dinic.hpp"

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
        Dinic<long long> graph(3);
        const int first = graph.add_edge(0, 1, 5);
        const int second = graph.add_edge(1, 2, 5);
        assert(first == 0);
        assert(second == 2);
        assert(graph.edges[static_cast<std::size_t>(first)].cap == 5);
        assert(graph.max_flow(0, 2, 3) == 3);
        assert(graph.edges[static_cast<std::size_t>(first)].flow == 3);
        assert(graph.edges[static_cast<std::size_t>(first ^ 1)].flow == -3);
        assert((graph.min_cut(0) == std::vector<char>{1, 1, 1}));
        assert(graph.max_flow(0, 2) == 2);
        assert((graph.min_cut(0) == std::vector<char>{1, 0, 0}));
    }
    {
        bool thrown = false;
        try{
            [[maybe_unused]] Dinic<int> graph(-1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        Dinic<int> graph(2);
        thrown = false;
        try{
            (void)graph.add_edge(0, 2, 1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        thrown = false;
        try{
            (void)graph.min_cut(-1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        Dinic<long long> graph(4);
        graph.add_edge(0, 1, 3);
        graph.add_edge(0, 2, 2);
        graph.add_edge(1, 2, 1);
        graph.add_edge(1, 3, 2);
        graph.add_edge(2, 3, 4);
        assert(graph.max_flow(0, 3) == 5);
    }
    {
        Dinic<long long> graph(2);
        graph.add_edge(0, 0, 7);
        graph.add_edge(0, 1, 4);
        assert(graph.max_flow(0, 1) == 4);
    }
    {
        Dinic<long long> graph(1);
        assert(graph.max_flow(0, 0, 123) == 0);
        bool thrown = false;
        try{
            (void)graph.max_flow(0, 0, -1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    std::mt19937 rng(20260820);
    for(int n = 2; n <= 10; n++){
        for(int step = 0; step < 80; step++){
            int s = 0, t = n - 1;
            Dinic<long long> graph(n);
            std::vector<std::tuple<int, int, long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u != v && rng() % 5 == 0){
                        long long c = static_cast<int>(rng() % 10);
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
    Dinic<long long> graph(n);
    for(int i = 0; i < m; i++){
        int u, v;
        long long c;
        std::cin >> u >> v >> c;
        graph.add_edge(u, v, c);
    }
    std::cout << graph.max_flow(s, t) << '\n';
}
